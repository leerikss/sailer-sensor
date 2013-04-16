(add-to-list 'load-path "~/elisp")

;; Show highlight 
(transient-mark-mode t)

;; do not make backup files
(setq make-backup-files nil)

;; Fix fucked up indentation
(setq-default c-basic-offset 2)
(setq c-default-style "linux"
          c-basic-offset 2)

;; Match parenthesis in code
(show-paren-mode 1)

;; Show column numbers
(column-number-mode t)

;; Show linenumbers
(require 'linum)
(global-linum-mode 1)

;; Smoother scrolling
;; (setq scroll-step 1 scroll-conservatively 10000)