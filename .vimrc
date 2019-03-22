syntax on
set number
filetype on
set history=1000
set nocompatible
set shiftwidth=4
color evening
syntax on
set autoindent
set smartindent
set tabstop=4
set guioptions-=T
set vb t_vb=
set ruler
set expandtab
imap{ {}<ESC>i<CR><ESC>O



set cursorline
hi Cursorline  cterm=NONE ctermbg=white ctermfg=black guibg=green guifg=green
set cursorcolumn
hi Cursorcolumn cterm=NONE ctermbg=white ctermfg=black guibg=green guifg=green
set mouse=a
set autoindent
set cindent
set noeb
set showmatch 
set clipboard+=unnamed
set nohls

set completeopt=preview,menu
set completeopt=longest,menu
set incsearch
if has("vims")
set nobackup
else
set nobackup
endif
inoremap ( ()<ESC>i
inoremap [ []<ESC>i
inoremap { {}<ESC>i
inoremap " ""<ESC>i
inoremap ' ''<ESC>i



