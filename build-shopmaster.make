; This is here so you can easily build shopmaster aegir after downloading shop_provision.
; Do not add anything here, add to github.com/devudo/shopmaster/shopmaster.make

core = 6.x
api = 2

projects[drupal][type] = "core"

projects[shopmaster][type] = "profile"
projects[shopmaster][download][type] = "git"
projects[shopmaster][download][url] = "git@github.com:devudo/shopmaster.git"
projects[shopmaster][download][branch] = "6.x-1.x-profile"