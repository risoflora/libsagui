/*                         _
 *   ___  __ _  __ _ _   _(_)
 *  / __|/ _` |/ _` | | | | |
 *  \__ \ (_| | (_| | |_| | |
 *  |___/\__,_|\__, |\__,_|_|
 *             |___/
 *
 *   –– an ideal C library to develop cross-platform HTTP servers.
 *
 * Copyright (c) 2016-2018 Silvio Clecio <silvioprog@gmail.com>
 *
 * This file is part of Sagui library.
 *
 * Sagui library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sagui library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Sagui library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <sagui.h>

/* NOTE: Some error checking has been omitted for clarity. */

/*
 * Simple example using TLS client authentication.
 *
 * Client example using cURL:
 *
 * curl -k --cert certs/client.p12 --pass abc123 --cert-type p12 https://localhost:<PORT>
 *
 * Certificate generation:
 *
 * ## CA
 * certtool --generate-privkey --outfile ca.key
 * echo 'cn = GnuTLS test CA' > ca.tmpl
 * echo 'ca' >> ca.tmpl
 * echo 'cert_signing_key' >> ca.tmpl
 * echo 'expiration_days = 3650' >> ca.tmpl
 * certtool --generate-self-signed --load-privkey ca.key --template ca.tmpl --outfile ca.pem
 *
 * ## Server
 * certtool --generate-privkey --outfile server.key
 * echo 'organization = GnuTLS test server' > server.tmpl
 * echo 'cn = test.gnutls.org' >> server.tmpl
 * echo 'tls_www_server' >> server.tmpl
 * echo 'expiration_days = 3650' >> server.tmpl
 * certtool --generate-certificate --load-ca-privkey ca.key --load-ca-certificate ca.pem --load-privkey server.key --template server.tmpl --outfile server.pem
 *
 * ## Client
 * certtool --generate-privkey --outfile client.key
 * echo 'cn = GnuTLS test client' > client.tmpl
 * echo 'tls_www_client' >> client.tmpl
 * echo 'expiration_days = 3650' >> client.tmpl
 * certtool --generate-certificate --load-ca-certificate ca.pem --load-ca-privkey ca.key --load-privkey client.key --template client.tmpl --outfile client.pem
 * certtool --to-p12 --p12-name=MyKey --password=abc123 --load-ca-certificate ca.pem --load-privkey client.key --load-certificate client.pem --outder --outfile client.p12
 */

#define KEY_FILE SG_EXAMPLES_CERTS_DIR "/server.key"
#define CERT_FILE SG_EXAMPLES_CERTS_DIR "/server.pem"
#define CA_FILE SG_EXAMPLES_CERTS_DIR "/ca.pem"

#define ERR_SIZE 256
#define PAGE_FMT "<html><head><title>Hello world</title></head><body><font color=\"%s\">%s</font></font></body></html>"
#define SECRET_MSG "Secret"

static void concat(char *s1, ...) {
    va_list ap;
    const char *s;
    va_start(ap, s1);
    while ((s = va_arg(ap, const char *)))
        strcat(s1, s);
    va_end(ap);
}

static bool sess_verify_cert(gnutls_session_t tls_session, const char *line_break, char *err) {
    gnutls_x509_crt_t cert = NULL;
    const gnutls_datum_t *certs;
    size_t len;
    unsigned int status, certs_size;
    int ret;
    if (!tls_session || !line_break || !err) {
        sg_strerror(EINVAL, err, ERR_SIZE);
        return false;
    }
    if ((ret = gnutls_certificate_verify_peers2(tls_session, &status)) != GNUTLS_E_SUCCESS) {
        concat(err, "Error verifying peers: ", gnutls_strerror(ret), line_break, NULL);
        goto fail;
    }
    if (status & GNUTLS_CERT_INVALID)
        concat(err, "The certificate is not trusted", line_break, NULL);
    if (status & GNUTLS_CERT_SIGNER_NOT_FOUND)
        concat(err, "The certificate has not got a known issuer", line_break, NULL);
    if (status & GNUTLS_CERT_REVOKED)
        concat(err, "The certificate has been revoked", line_break, NULL);
    if (gnutls_certificate_type_get(tls_session) != GNUTLS_CRT_X509) {
        concat(err, "The certificate type is not X.509", line_break, NULL);
        goto fail;
    }
    if ((ret = gnutls_x509_crt_init(&cert)) != GNUTLS_E_SUCCESS) {
        concat(err, "Error in the certificate initialization: ", gnutls_strerror(ret), line_break, NULL);
        goto fail;
    }
    if (!(certs = gnutls_certificate_get_peers(tls_session, &certs_size))) {
        concat(err, "No certificate was found", line_break, NULL);
        goto fail;
    }
    if ((ret = gnutls_x509_crt_import(cert, &certs[0], GNUTLS_X509_FMT_DER)) != GNUTLS_E_SUCCESS) {
        concat(err, "Error parsing certificate: ", gnutls_strerror(ret), line_break, NULL);
        goto fail;
    }
    if (gnutls_x509_crt_get_expiration_time(cert) < time(NULL)) {
        concat(err, "The certificate has expired", line_break, NULL);
        goto fail;
    }
    if (gnutls_x509_crt_get_activation_time(cert) > time(NULL)) {
        concat(err, "The certificate has not been activated yet", line_break, NULL);
        goto fail;
    }
fail:
    len = strlen(err);
    err[len - strlen("<br>")] = '\0';
    gnutls_x509_crt_deinit(cert);
    return len == 0;
}

static void req_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_httpreq *req, struct sg_httpres *res) {
    char msg[ERR_SIZE];
    char *color, *page;
    size_t page_size;
    unsigned int status;
    if (sess_verify_cert(sg_httpreq_tls_session(req), "<br>", msg)) {
        strcpy(msg, SECRET_MSG);
        color = "green";
        status = 200;
    } else {
        color = "red";
        status = 500;
    }
    page_size = (size_t) snprintf(NULL, 0, PAGE_FMT, color, msg);
    page = sg_alloc(page_size);
    snprintf(page, page_size, PAGE_FMT, color, msg);
    sg_httpres_send(res, page, "text/html; charset=utf-8", status);
    sg_free(page);
}

int main(void) {
    struct sg_httpsrv *srv = sg_httpsrv_new(req_cb, NULL);
    gnutls_datum_t key_file, cert_file, ca_file;
    int ret, status = EXIT_FAILURE;
    memset(&key_file, 0, sizeof(gnutls_datum_t));
    memset(&cert_file, 0, sizeof(gnutls_datum_t));
    memset(&ca_file, 0, sizeof(gnutls_datum_t));
    if ((ret = gnutls_load_file(KEY_FILE, &key_file)) != GNUTLS_E_SUCCESS) {
        fprintf(stderr, "Error loading the private key \"%s\": %s\n", KEY_FILE, gnutls_strerror(ret));
        fflush(stdout);
        goto fail;
    }
    if ((ret = gnutls_load_file(CERT_FILE, &cert_file)) != GNUTLS_E_SUCCESS) {
        fprintf(stderr, "Error loading the certificate \"%s\": %s\n", CERT_FILE, gnutls_strerror(ret));
        fflush(stdout);
        goto fail;
    }
    if ((ret = gnutls_load_file(CA_FILE, &ca_file)) != GNUTLS_E_SUCCESS) {
        fprintf(stderr, "Error loading the CA \"%s\": %s\n", CA_FILE, gnutls_strerror(ret));
        fflush(stdout);
        goto fail;
    }
    if (sg_httpsrv_tls_listen2(srv, (const char *) key_file.data, NULL, (const char *) cert_file.data,
                               (const char *) ca_file.data, NULL, 0 /* 0 = port chosen randomly */, false)) {
        status = EXIT_SUCCESS;
        fprintf(stdout, "Server running at https://localhost:%d\n", sg_httpsrv_port(srv));
        fflush(stdout);
        getchar();
    }
fail:
    sg_httpsrv_free(srv);
    if (key_file.size > 0)
        gnutls_free(key_file.data);
    if (cert_file.size > 0)
        gnutls_free(cert_file.data);
    if (ca_file.size > 0)
        gnutls_free(ca_file.data);
    return status;
}
