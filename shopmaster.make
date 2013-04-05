; NOTE!  Since the name shopmaster matches the project (profile),
; anything in here goes into profiles/shopmaster/modules etc!

core = 6.x
api = 2

projects[drupal][type] = "core"

; We removed all contrib stuff from this makefile because the makefile isn't good
; for deploying updates.  You would have to drush make a new platform and then
; migrate the existing site.
;
; Instead, please put all modules into the shopmaster repo itself, and we will
; update with git-pull-recursive
