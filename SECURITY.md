# Security Policy

ptusa_main is control software for industrial automation systems, so we
take security problems in it seriously. We welcome reports from users,
integrators and researchers, and we work with reporters to resolve
confirmed problems.

## Reporting a Vulnerability

Report security vulnerabilities privately through GitHub, not through
a public issue. Open a private security vulnerability report at:

<https://github.com/sav-oss-org/ptusa_main/security/advisories/new>

Please do not open a public issue, discussion or pull request that
describes the vulnerability before a fix is available.

A useful report includes:

* a description of the vulnerability and its potential impact;
* the affected release tag or commit, and the relevant part of the
  source code, if you know it;
* step-by-step instructions to reproduce the problem;
* the platform, build type and build options you used, if relevant.

Please keep the details confidential until we publish a fix.

## Supported Versions

We provide security fixes for the latest CalVer release and for its
same-day `.N` hotfix tags. Release tags use the CalVer form
`YYYY.MM.DD.N` and are documented in
[`docs/CONTRIBUTING.md`](docs/CONTRIBUTING.md); the current release is
listed on the
[releases page](https://github.com/sav-oss-org/ptusa_main/releases).

Reports against other releases or against `master` are still welcome.
We will confirm the problem against a supported version before we
schedule a fix.

## What to Expect

We aim to acknowledge a new report within about two weeks and to keep
you informed while we investigate and prepare a fix. These are
best-effort timelines, not a contractual commitment. We will tell you
when a fix is published and, when appropriate, we will publish a
GitHub security advisory describing the problem and the fixed
versions.

## Recognition

Thank you for helping keep ptusa_main and its users safe. We are
grateful to everyone who reports a problem responsibly, and we are
glad to credit you in the security advisory for the fix, if you would
like that.
