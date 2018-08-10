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

#include <stdio.h>
#include <stdlib.h>
#include <sagui.h>

/* NOTE: Error checking has been omitted for clarity. */

/*
 * Very simple insecure HTTPS server. For total security, use: https://help.ubuntu.com/community/OpenSSL.
 *
 * Client example using cURL:
 *
 * curl -k https://localhost:<PORT>
 *
 * Certificate generation:
 *
 * # Private key
 * certtool --generate-privkey --outfile server.key
 * echo 'organization = GnuTLS test server' > server.tmpl
 * echo 'cn = test.gnutls.org' >> server.tmpl
 * echo 'tls_www_server' >> server.tmpl
 * echo 'expiration_days = 3650' >> server.tmpl
 *
 * # Certificate
 * certtool --generate-self-signed --load-privkey server.key --template server.tmpl --outfile server.pem
 */

const char private_key[] =
    "-----BEGIN RSA PRIVATE KEY-----\n"
    "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\n"
    "-----END RSA PRIVATE KEY-----";

const char certificate[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEODCCAqCgAwIBAgIMW23FvhiQ/Xip31BxMA0GCSqGSIb3DQEBCwUAMDcxGDAW\n"
    "BgNVBAMTD3Rlc3QuZ251dGxzLm9yZzEbMBkGA1UEChMSR251VExTIHRlc3Qgc2Vy\n"
    "dmVyMB4XDTE4MDgxMDE3MDUwMloXDTI4MDgwNzE3MDUwMlowNzEYMBYGA1UEAxMP\n"
    "dGVzdC5nbnV0bHMub3JnMRswGQYDVQQKExJHbnVUTFMgdGVzdCBzZXJ2ZXIwggGi\n"
    "MA0GCSqGSIb3DQEBAQUAA4IBjwAwggGKAoIBgQDWrNobZBvCcHNtFgQ0luHXsGg0\n"
    "zVchXg8fTfQUkvvsFH4p8v+p9TBjo2TPL+IO2fUFitSZ3wy6zfihuwLRp3+whqtq\n"
    "KUr/+YBhXKtUe4uA4nhzUW6yoy9p8QHrsssXF4i3ArVfvxpP1NIRs5mqm5VeYQ87\n"
    "RW97nedAdpGvvMFtvSPy0GrEJzTvJb1NGWg5y2GiVhKnWY+JoAnl39ghyZIR4Po0\n"
    "GkxnVWC4cglvrmLjkTNf42UkdJWNxWCrUCnmCDr5bZJjpNJKNtuccA2ahUym7r3S\n"
    "M/o1cMg5jkgiecxKT6rFvifADxr+B4sqigoi3Dzm06Xbtv5qnfGPANlKuLFiXmJ5\n"
    "FqTK6cGKmcT1tLPR2aXcqI8z7qFsCuyQ+58PeCMeMGFCqWq8DSKacaufj+vCZpMW\n"
    "mUu/eQJ/JMSVBwxHuKQXm502fjJnMPMDuB5Y8E5nZd42Qrl/5Qs5+m/FmxhFoJIM\n"
    "s+Ijb/DCMu9KWPrPSaY6eJ+vg0V2n94V89A8KgcCAwEAAaNEMEIwDAYDVR0TAQH/\n"
    "BAIwADATBgNVHSUEDDAKBggrBgEFBQcDATAdBgNVHQ4EFgQUxH1HUKpvYFEHrPeJ\n"
    "sY0I7HQDbIIwDQYJKoZIhvcNAQELBQADggGBABDTlhiKuuh51Rx+mpt5vjJ7zXRJ\n"
    "1RoHY92AmZY49hfdpUp6mMLvbRdD6REv4pe1IORGsqgPk4MPCQsaFGbpZS1CokLz\n"
    "Sex4LZruHYWtv18fUFliJIZZSITnArNB29lXAem5T20D04bCCYLJJB3VTcPilbkf\n"
    "ipT/h1CyhbWX14ZtkzzpWMAwLgod6uZvJqJXTpjwdWA7Anp4yfh2QxBYC5/us4xP\n"
    "wHa0euWOBZ+Q9ZNZ/fFdLESLSbBob9736hBglNSgBFCMNezqs18/EGIJcS7w96PN\n"
    "YJtVsVhcQJMMT4dnaSM/Ri4CPv7j8/zll6uq4kHpxZLhuxeRSeuKBn6jl0wHQFd1\n"
    "7bpHrRLBuRyDhPWrzdmMY2dyJ5DkO39auisAyJza8IddfNnCa7howSjp/ZvZN9Sf\n"
    "gi1klZeSpe+iijWQaxjIKAr/g8Rn+ALfeMAitm6DjCcTUkXdKtXVqTwdFZRNxrNH\n"
    "lqt+H07raUsv/p50oVS6/Euv8fBm3EKPwxC64w==\n"
    "-----END CERTIFICATE-----";

static void req_cb(__SG_UNUSED void *cls, __SG_UNUSED struct sg_httpreq *req, struct sg_httpres *res) {
    sg_httpres_send(res,
                    "<html><head><title>Hello world</title></head><body>Hello <font color=\"green\">HTTPS</font></body></html>",
                    "text/html; charset=utf-8", 200);
}

int main(void) {
    struct sg_httpsrv *srv = sg_httpsrv_new(req_cb, NULL);
    if (!sg_httpsrv_tls_listen(srv, private_key, certificate, 0 /* 0 = port chosen randomly */, false)) {
        sg_httpsrv_free(srv);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Server running at https://localhost:%d\n", sg_httpsrv_port(srv));
    fflush(stdout);
    getchar();
    sg_httpsrv_free(srv);
    return EXIT_SUCCESS;
}
