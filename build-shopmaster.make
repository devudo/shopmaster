;
; Use this makefile to build shopmaster properly.
; Download just this file, then run
; $ drush make build-shopmaster.make /path/to/target
;
; NOTE: This will only rebuild what is pushed to the git repo, not
; local changes to shopmaster.make!
;
; Keep this file up to date with https://github.com/devudo/devudo.github.com / shopmaster.make
; This way it is available online.
;
; Install shopmaster like so: (after SSH access to github is granted)
; $ drush make http://devudo.github.com/build-shopmaster.make /path/to/target

core = 6.x
api = 2

projects[drupal][type] = "core"

projects[shopmaster][type] = "profile"
projects[shopmaster][download][type] = "git"
projects[shopmaster][download][url] = "git@github.com:devudo/shopmaster.git"
projects[shopmaster][download][branch] = "6.x-1.x-profile"