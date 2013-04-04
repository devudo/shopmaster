;
; This makefile should be used by devmaster-install
; 

core = 6.x
api = 2

projects[drupal][type] = "core"

projects[shopmaster][type] = "profile"
projects[shopmaster][download][type] = "git"
projects[shopmaster][download][url] = "git@github.com:devudo/shopmaster.git"
projects[shopmaster][download][branch] = "6.x-1.x"