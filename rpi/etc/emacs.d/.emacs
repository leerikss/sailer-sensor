;; Where additional plugins are found
(add-to-list 'load-path "~/elisp")

;; Show highlight 
(transient-mark-mode t)

;; do not make backup files
(setq make-backup-files nil)

;; Custom indentation
(defun my-indentation ()
  (setq c-basic-offset 2)
  (c-set-offset 'substatement-open 0)
)

;; Java & C indentation
(add-hook 'c-mode-common-hook 'my-indentation)
(add-hook 'java-mode-hook 'my-indentation)

;; Spaces for tabs
(setq c-basic-indent 2)
(setq tab-width 2)
(setq indent-tabs-mode nil)

;; Match parenthesis in code
(show-paren-mode 1)

;; Show column numbers
(column-number-mode t)

;; Show linenumbers
(require 'linum)
(global-linum-mode 1)

;; CSS-mode
(autoload 'css-mode "css-mode" "Mode for editing CSS files" t)
(setq auto-mode-alist
      (append '(("\\.css$" . css-mode))
	      auto-mode-alist))

;; Smoother scrolling
;; (setq scroll-step 1 scroll-conservatively 10000)
