dte Release Checklist
=====================

1. Create release commit
   1. Update `CHANGELOG.md`
   2. Hard code `VERSION` variable in `mk/build.mk` to release version
   3. Update `RELEASE_VERSIONS` in `mk/dev.mk`
   4. Remove `-g` from default `CFLAGS`
   5. Update tarball name in `README.md`
   6. Check `make vars` output
   7. `git commit -m "Prepare v${VER} release"`

2. Tag and upload
   1. `git tag -s -m "Release v${VER}" v${VER} ${COMMIT}`
   2. `make dist`
   3. Upload tarball and detached GPG signature to GitLab pages
   4. Wait for GitLab pages job to finish
   5. Check tarball link in `README.md` works
   6. Push tag to remotes

3. Create post-release commit
   1. Update `mk/sha256sums.txt`
   2. Reset `VERSION` and `CFLAGS`
