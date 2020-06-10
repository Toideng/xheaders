"set runtimepath=~/.vim,/usr/share/vim/vimfiles,/usr/share/vim/vim81/,/usr/share/vim/vimfiles/after,~/.vim/after

set nobomb

" disable octal arithmetic (it's annoying as hell)
set nrformats-=octal

syntax on
"syntax off
set tabstop=8
set softtabstop=0
set shiftwidth=8
set autoindent
set nosmartindent
set nocindent
set copyindent

set modeline
set showcmd

" Uncomment following line to see both tabs and spaces
set list
"set listchars=tab:>\ 
set listchars=tab:>\ ,space:·,conceal:X
highlight SpecialKey ctermfg=238



" Line numbering options:
set nu " Line numbering throughout the whole file
set rnu " Relative line numbering



set ruler



" vimdiff colors
highlight DiffText   ctermbg=88 cterm=bold,underline
highlight DiffChange ctermbg=52
highlight DiffAdd    ctermbg=22
highlight DiffDelete ctermbg=28



" Make text more readable
" full white
"highlight Normal ctermfg=15
" tinted gray
highlight Normal ctermfg=252



highlight ColorColumn ctermbg=235
let &colorcolumn=join(range(81,999),",")

set cursorline
highlight CursorLine   cterm=NONE ctermbg=235
highlight CursorLineNr cterm=NONE





set backspace=indent,eol,start





" Set the postgres path for autocompletion
if isdirectory('/usr/local/pgsql/include')
	set path+=/usr/local/pgsql/include
endif




function! PrefillLine(key) abort
	let lnum1 = line('.') + (a:key ==# 'O' ? -1 : 0)
	let lnum2 = lnum1 - 1
	let line1 = getline(lnum1)
	let line2 = getline(lnum2)

	let prefill = ''
	let i = 0
	while line1[i] ==# line2[i] && i < len(line1)
		let prefill.= line1[i]
		let i+= 1
	endwhile

	if prefill !~ '^\s*$'
		return a:key . "\<C-o>0\<C-o>D" . prefill
	endif

	return a:key
endfunction

inoremap <expr> <CR> PrefillLine('<CR>')
nnoremap <expr> O PrefillLine('O')
nnoremap <expr> o PrefillLine('o')
