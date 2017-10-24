set tabstop=4
set softtabstop=4
set shiftwidth=4
set noexpandtab
set colorcolumn=110
highlight ColorColumn ctermbg=darkgray
augroup project
	autocmd!
	autocmd BufRead, BufNewFile *.h, *.cpp 
augroup END
let &path.="App,OpenGL,"