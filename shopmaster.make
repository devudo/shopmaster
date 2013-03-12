; NOTE!  Since the name shopmaster matches the project (profile), anything in here goes into profiles/shopmaster/modules etc!

core = 6.x
api = 2

projects[drupal][type] = "core"

; shop_hosting
projects[shop_hosting][type] = "module"
projects[shop_hosting][download][type] = "git"
projects[shop_hosting][download][url] = "git@github.com:devudo/shop_hosting.git"
projects[shop_hosting][download][branch] = "master"

; Contrib
projects[sshkey][type] = "module"
projects[cck][type] = "module"
projects[features][type] = "module"
projects[views][type] = "module"

; HOSTMASTER CORE
; This automatically inherits hostmaster's make file
; we don't have to worry about anything already included in hostmaster
projects[hostmaster][type] = "profile"
