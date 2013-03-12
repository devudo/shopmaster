;
; Use this makefile to build shopmaster properly.
; Download just this file, then run
; $ drush make build-shopmaster.make /path/to/target
;
; NOTE: This will only rebuild what is pushed to the git repo, not
; local changes to shopmaster.make!
;

core = 6.x
api = 2

projects[drupal][type] = "core"

projects[shopmaster][type] = "profile"
projects[shopmaster][download][type] = "git"
projects[shopmaster][download][url] = "git@github.com:devudo/shopmaster.git"
projects[shopmaster][download][branch] = "6.x-1.x-profile"