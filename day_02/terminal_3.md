# Exercise 2.3 Terminal Outputs

## 1. curl: -I [fetch headers only], -v [verbose], force tls v1.3

```bash
curl -Iv --tlsv1.3 https://www.cloudflare.com
```

### Notes

1. ALPN negotiation

```text
* ALPN, offering h2
* ALPN, offering http/1.1
...
* ALPN, server accepted to use h2
```

2. Questions I had after seeing this and reading the explanation

```text
why are there so many TLS(IN) and TLS(OUT) for handshake when supposedly it's very much optimized
how do we infer it's encrypted after Server hello
what do the numbers inside parentheses (22), (1) mean
what does Finished mean
what do *, >, <, and no prefix mean ?
```

3. TLS handshake (TLSv1.3) achieves 1-RTT (Round Trip Time)

4. HTTP/2 103 Status Code is `Early Hints` - it lets the server send resource hints to a browser before the server is ready with the final HTML body. In this case, Cloudflare's servers are sending us fonts and some json files.

## Full Output Body

```text
*   Trying 104.16.124.96:443...
* Connected to www.cloudflare.com (104.16.124.96) port 443 (#0)
* ALPN, offering h2
* ALPN, offering http/1.1
*  CAfile: /etc/ssl/certs/ca-certificates.crt
*  CApath: /etc/ssl/certs
* TLSv1.0 (OUT), TLS header, Certificate Status (22):
* TLSv1.3 (OUT), TLS handshake, Client hello (1):
* TLSv1.2 (IN), TLS header, Certificate Status (22):
* TLSv1.3 (IN), TLS handshake, Server hello (2):
* TLSv1.2 (IN), TLS header, Finished (20):
* TLSv1.2 (IN), TLS header, Supplemental data (23):
* TLSv1.3 (IN), TLS handshake, Encrypted Extensions (8):
* TLSv1.3 (IN), TLS handshake, Certificate (11):
* TLSv1.3 (IN), TLS handshake, CERT verify (15):
* TLSv1.3 (IN), TLS handshake, Finished (20):
* TLSv1.2 (OUT), TLS header, Finished (20):
* TLSv1.3 (OUT), TLS change cipher, Change cipher spec (1):
* TLSv1.2 (OUT), TLS header, Supplemental data (23):
* TLSv1.3 (OUT), TLS handshake, Finished (20):
* SSL connection using TLSv1.3 / TLS_AES_256_GCM_SHA384
* ALPN, server accepted to use h2
* Server certificate:
*  subject: CN=www.cloudflare.com
*  start date: Sep  3 07:35:04 2026 GMT
*  expire date: Dec  2 07:35:03 2026 GMT
*  subjectAltName: host "www.cloudflare.com" matched cert's "www.cloudflare.com"
*  issuer: C=US; O=Let's Encrypt; CN=YE2
*  SSL certificate verify ok.
* Using HTTP2, server supports multiplexing
* Connection state changed (HTTP/2 confirmed)
* Copying HTTP/2 data in stream buffer to connection buffer after upgrade: len=0
* TLSv1.2 (OUT), TLS header, Supplemental data (23):
* TLSv1.2 (OUT), TLS header, Supplemental data (23):
* TLSv1.2 (OUT), TLS header, Supplemental data (23):
* Using Stream ID: 1 (easy handle 0x637cdea7aa90)
* TLSv1.2 (OUT), TLS header, Supplemental data (23):
> HEAD / HTTP/2
> Host: www.cloudflare.com
> user-agent: curl/7.81.0
> accept: */*
>
* TLSv1.2 (IN), TLS header, Supplemental data (23):
* TLSv1.3 (IN), TLS handshake, Newsession Ticket (4):
* TLSv1.3 (IN), TLS handshake, Newsession Ticket (4):
* old SSL session ID is stale, removing
* TLSv1.2 (IN), TLS header, Supplemental data (23):
* TLSv1.2 (OUT), TLS header, Supplemental data (23):
* TLSv1.2 (IN), TLS header, Supplemental data (23):
* TLSv1.2 (IN), TLS header, Supplemental data (23):
< HTTP/2 103
HTTP/2 103
< link: </fonts/Kunst%20Grotesk%20Regular.woff2>; as=font; crossorigin; rel=preload; type=font/woff2, </fonts/Kunst%20Grotesk%20Medium.woff2>; as=font; crossorigin; rel=preload; type=font/woff2, <https://ot.www.cloudflare.com>; crossorigin; rel=preconnect, <https://imagedelivery.net>; crossorigin; rel=preconnect, </static/hero-poster.avif>; as=image; fetchpriority=high; rel=preload; type=image/avif
link: </fonts/Kunst%20Grotesk%20Regular.woff2>; as=font; crossorigin; rel=preload; type=font/woff2, </fonts/Kunst%20Grotesk%20Medium.woff2>; as=font; crossorigin; rel=preload; type=font/woff2, <https://ot.www.cloudflare.com>; crossorigin; rel=preconnect, <https://imagedelivery.net>; crossorigin; rel=preconnect, </static/hero-poster.avif>; as=image; fetchpriority=high; rel=preload; type=image/avif

* TLSv1.2 (IN), TLS header, Supplemental data (23):
* TLSv1.2 (IN), TLS header, Supplemental data (23):
* TLSv1.2 (IN), TLS header, Supplemental data (23):
< HTTP/2 200
HTTP/2 200
< date: Tue, 15 Sep 2026 16:09:06 GMT
date: Tue, 15 Sep 2026 16:09:06 GMT
< content-type: text/html; charset=utf-8
content-type: text/html; charset=utf-8
< link: <https://www.cloudflare.com/.well-known/agents.json>; rel="api-catalog", <https://www.cloudflare.com/.well-known/webmcp.json>; rel="service-desc", <https://www.cloudflare.com/openapi.json>; rel="service-desc", <https://www.cloudflare.com/llms.txt>; rel="service-doc", <https://www.cloudflare.com/sitemap.xml>; rel="sitemap", </fonts/Kunst%20Grotesk%20Regular.woff2>; rel=preload; as=font; type="font/woff2"; crossorigin, </fonts/Kunst%20Grotesk%20Medium.woff2>; rel=preload; as=font; type="font/woff2"; crossorigin, <https://ot.www.cloudflare.com>; rel=preconnect; crossorigin, <https://imagedelivery.net>; rel=preconnect; crossorigin, </static/hero-poster.avif>; rel=preload; as=image; type="image/avif"; fetchpriority=high
link: <https://www.cloudflare.com/.well-known/agents.json>; rel="api-catalog", <https://www.cloudflare.com/.well-known/webmcp.json>; rel="service-desc", <https://www.cloudflare.com/openapi.json>; rel="service-desc", <https://www.cloudflare.com/llms.txt>; rel="service-doc", <https://www.cloudflare.com/sitemap.xml>; rel="sitemap", </fonts/Kunst%20Grotesk%20Regular.woff2>; rel=preload; as=font; type="font/woff2"; crossorigin, </fonts/Kunst%20Grotesk%20Medium.woff2>; rel=preload; as=font; type="font/woff2"; crossorigin, <https://ot.www.cloudflare.com>; rel=preconnect; crossorigin, <https://imagedelivery.net>; rel=preconnect; crossorigin, </static/hero-poster.avif>; rel=preload; as=image; type="image/avif"; fetchpriority=high
< cache-control: no-store
cache-control: no-store
< vary: accept-encoding
vary: accept-encoding
< strict-transport-security: max-age=31536000; includeSubDomains
strict-transport-security: max-age=31536000; includeSubDomains
< content-security-policy: default-src 'self'; script-src 'self' 'unsafe-inline' 'unsafe-eval' https://static.cloudflareinsights.com https://static-staging.cloudflareinsights.com https://challenges.cloudflare.com https://*.onetrust.com https://cdn.cookielaw.org https://ot.www.cloudflare.com https://www.googletagmanager.com https://tagmanager.google.com https://www.googleadservices.com https://googleads.g.doubleclick.net https://adservice.google.com https://cdn.bizible.com https://js.adsrvr.org https://*.marketo.net https://platform.twitter.com https://static.ads-twitter.com https://scripts.demandbase.com https://tag.demandbase.com https://*.6sc.co https://*.qualified.com https://snap.licdn.com https://bat.bing.com; style-src 'self' 'unsafe-inline'; img-src 'self' data: https:; font-src 'self' data:; connect-src 'self' https://*.googletagmanager.com https://*.google-analytics.com https://*.google.com https://*.doubleclick.net https://www.googleadservices.com https://translate.googleapis.com https://cdn.bizible.com https://js.adsrvr.org https://*.marketo.net https://ads-twitter.com https://analytics.twitter.com https://*.twimg.com https://api.demandbase.com https://scripts.demandbase.com https://tag.demandbase.com https://tag-logger.demandbase.com https://api.company-target.com https://*.6sc.co https://epsilon.6sense.com https://*.qualified.com https://*.ads.linkedin.com https://www.linkedin.com https://bat.bing.com https: wss://*.qualified.com wss://*.registrar.cloudflare.com; frame-src https://*.adsrvr.org https://*.cloudflare.com https://*.videodelivery.net https://*.cloudflarestream.com https://www.googletagmanager.com https://*.qualified.com https://td.doubleclick.net https://bid.g.doubleclick.net https://9309168.fls.doubleclick.net https://9973066.fls.doubleclick.net https://s.company-target.com; object-src 'none'; base-uri 'self'; form-action 'self'; frame-ancestors 'none'; worker-src 'self' blob:; child-src 'self' blob:; upgrade-insecure-requests
content-security-policy: default-src 'self'; script-src 'self' 'unsafe-inline' 'unsafe-eval' https://static.cloudflareinsights.com https://static-staging.cloudflareinsights.com https://challenges.cloudflare.com https://*.onetrust.com https://cdn.cookielaw.org https://ot.www.cloudflare.com https://www.googletagmanager.com https://tagmanager.google.com https://www.googleadservices.com https://googleads.g.doubleclick.net https://adservice.google.com https://cdn.bizible.com https://js.adsrvr.org https://*.marketo.net https://platform.twitter.com https://static.ads-twitter.com https://scripts.demandbase.com https://tag.demandbase.com https://*.6sc.co https://*.qualified.com https://snap.licdn.com https://bat.bing.com; style-src 'self' 'unsafe-inline'; img-src 'self' data: https:; font-src 'self' data:; connect-src 'self' https://*.googletagmanager.com https://*.google-analytics.com https://*.google.com https://*.doubleclick.net https://www.googleadservices.com https://translate.googleapis.com https://cdn.bizible.com https://js.adsrvr.org https://*.marketo.net https://ads-twitter.com https://analytics.twitter.com https://*.twimg.com https://api.demandbase.com https://scripts.demandbase.com https://tag.demandbase.com https://tag-logger.demandbase.com https://api.company-target.com https://*.6sc.co https://epsilon.6sense.com https://*.qualified.com https://*.ads.linkedin.com https://www.linkedin.com https://bat.bing.com https: wss://*.qualified.com wss://*.registrar.cloudflare.com; frame-src https://*.adsrvr.org https://*.cloudflare.com https://*.videodelivery.net https://*.cloudflarestream.com https://www.googletagmanager.com https://*.qualified.com https://td.doubleclick.net https://bid.g.doubleclick.net https://9309168.fls.doubleclick.net https://9973066.fls.doubleclick.net https://s.company-target.com; object-src 'none'; base-uri 'self'; form-action 'self'; frame-ancestors 'none'; worker-src 'self' blob:; child-src 'self' blob:; upgrade-insecure-requests
< cross-origin-opener-policy: unsafe-none
cross-origin-opener-policy: unsafe-none
< cross-origin-resource-policy: cross-origin
cross-origin-resource-policy: cross-origin
< permissions-policy: geolocation=(), camera=(), microphone=()
permissions-policy: geolocation=(), camera=(), microphone=()
< referrer-policy: strict-origin-when-cross-origin
referrer-policy: strict-origin-when-cross-origin
< x-content-type-options: nosniff
x-content-type-options: nosniff
< x-frame-options: SAMEORIGIN
x-frame-options: SAMEORIGIN
< x-served-by: marketing-site
x-served-by: marketing-site
< x-xss-protection: 1; mode=block
x-xss-protection: 1; mode=block
< x-rm: GW
x-rm: GW
< set-cookie: _ga=GA1.1.605adfa6-64dc-44b9-b666-316afa0e95c3; Expires=Fri, 15 Oct 2027 16:09:06 GMT; Path=/
set-cookie: _ga=GA1.1.605adfa6-64dc-44b9-b666-316afa0e95c3; Expires=Fri, 15 Oct 2027 16:09:06 GMT; Path=/
< set-cookie: cfz_google-analytics_v4={"nzcr_ga4":{"v":"605adfa6-64dc-44b9-b666-316afa0e95c3","e":1823616546336}}; Expires=Fri Oct 15 2027 16:09:06 GMT+0000 (Coordinated Universal Time); HttpOnly; SameSite=Lax; Path=/; Secure
set-cookie: cfz_google-analytics_v4={"nzcr_ga4":{"v":"605adfa6-64dc-44b9-b666-316afa0e95c3","e":1823616546336}}; Expires=Fri Oct 15 2027 16:09:06 GMT+0000 (Coordinated Universal Time); HttpOnly; SameSite=Lax; Path=/; Secure
< set-cookie: cfz_adobe={"MsVJ_ecid":{"v":"CiY4MTgzMjQ5NDExNTQ0NTIzOTM1NTE3NzIxNTQwMTg2NjMxMTk0MVIOCJWF0a6KNCoDVkE2MALwAZWF0a6KNA==","e":1823616546535}}; Expires=Fri Oct 15 2027 16:09:06 GMT+0000 (Coordinated Universal Time); HttpOnly; SameSite=Lax; Path=/; Secure
set-cookie: cfz_adobe={"MsVJ_ecid":{"v":"CiY4MTgzMjQ5NDExNTQ0NTIzOTM1NTE3NzIxNTQwMTg2NjMxMTk0MVIOCJWF0a6KNCoDVkE2MALwAZWF0a6KNA==","e":1823616546535}}; Expires=Fri Oct 15 2027 16:09:06 GMT+0000 (Coordinated Universal Time); HttpOnly; SameSite=Lax; Path=/; Secure
< set-cookie: kndctr_8AD56F28618A50850A495FB6_AdobeOrg_identity=CiY4MTgzMjQ5NDExNTQ0NTIzOTM1NTE3NzIxNTQwMTg2NjMxMTk0MVIOCJWF0a6KNCoDVkE2MALwAZWF0a6KNA==; Expires=Fri, 15 Oct 2027 16:09:06 GMT; Path=/; Secure; SameSite=Lax
set-cookie: kndctr_8AD56F28618A50850A495FB6_AdobeOrg_identity=CiY4MTgzMjQ5NDExNTQ0NTIzOTM1NTE3NzIxNTQwMTg2NjMxMTk0MVIOCJWF0a6KNCoDVkE2MALwAZWF0a6KNA==; Expires=Fri, 15 Oct 2027 16:09:06 GMT; Path=/; Secure; SameSite=Lax
< set-cookie: __cf_bm=DelUXjp2OnmuJ.0dlEqLESZ8Q0oXK2ejUhkOqotbqMk-1789488546.3135922-1.0.1.1-.KzJQGndkdrvyt1ddl9jXE3l.AtOARB6XxPo4xxx30SGbBOXGSBpQGPdk7gguLiLBAabaxnsQHxT5eMwVIy.dl4g9vfoZWHozRZaxedi2zl1OhoPm4ZUnUzQkUAcF5HiTpESdq3XQOhhFUHA._uq1w; HttpOnly; SameSite=None; Secure; Path=/; Domain=www.cloudflare.com; Expires=Tue, 15 Sep 2026 16:39:06 GMT
set-cookie: __cf_bm=DelUXjp2OnmuJ.0dlEqLESZ8Q0oXK2ejUhkOqotbqMk-1789488546.3135922-1.0.1.1-.KzJQGndkdrvyt1ddl9jXE3l.AtOARB6XxPo4xxx30SGbBOXGSBpQGPdk7gguLiLBAabaxnsQHxT5eMwVIy.dl4g9vfoZWHozRZaxedi2zl1OhoPm4ZUnUzQkUAcF5HiTpESdq3XQOhhFUHA._uq1w; HttpOnly; SameSite=None; Secure; Path=/; Domain=www.cloudflare.com; Expires=Tue, 15 Sep 2026 16:39:06 GMT
< report-to: {"group":"cf-nel","max_age":604800,"endpoints":[{"url":"https://a.nel.cloudflare.com/report/v4?s=%2B0a0s5xyiCuuFvls5CSvmd7%2BKvemQlm2lPulHgh40mUEPIyzNu5Pt%2B5%2FiNYCZiqb4KGsMovSsN974Fqw2PhhFzVKwgGPNLwnKrn69GNc%2BxClPH5H0BU89Z13Df7EGzew%2BCKARA%3D%3D"}]}
report-to: {"group":"cf-nel","max_age":604800,"endpoints":[{"url":"https://a.nel.cloudflare.com/report/v4?s=%2B0a0s5xyiCuuFvls5CSvmd7%2BKvemQlm2lPulHgh40mUEPIyzNu5Pt%2B5%2FiNYCZiqb4KGsMovSsN974Fqw2PhhFzVKwgGPNLwnKrn69GNc%2BxClPH5H0BU89Z13Df7EGzew%2BCKARA%3D%3D"}]}
< nel: {"report_to":"cf-nel","success_fraction":0.0,"max_age":604800}
nel: {"report_to":"cf-nel","success_fraction":0.0,"max_age":604800}
< server: cloudflare
server: cloudflare
< cf-ray: a3b8e4d67eca40b7-BOM
cf-ray: a3b8e4d67eca40b7-BOM
< alt-svc: h3=":443"; ma=86400
alt-svc: h3=":443"; ma=86400

<
* Connection #0 to host www.cloudflare.com left intact
```
