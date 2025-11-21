let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Programming/Eggsh
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +1 main.c
badd +1 term://~/Programming/Eggsh//253233:/usr/bin/bash
badd +7 todo.md
badd +1 Makefile
badd +1 src/command_builder_and_tokenizer.c
badd +1 include/tokenizer.h
argglobal
%argdel
$argadd main.c
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit main.c
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd _ | wincmd |
split
wincmd _ | wincmd |
split
2wincmd k
wincmd w
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe '2resize ' . ((&lines * 13 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 118 + 119) / 238)
exe '3resize ' . ((&lines * 24 + 28) / 56)
exe 'vert 3resize ' . ((&columns * 118 + 119) / 238)
exe '4resize ' . ((&lines * 15 + 28) / 56)
exe 'vert 4resize ' . ((&columns * 118 + 119) / 238)
argglobal
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 8 - ((7 * winheight(0) + 27) / 54)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 8
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("todo.md", ":p")) | buffer todo.md | else | edit todo.md | endif
if &buftype ==# 'terminal'
  silent file todo.md
endif
balt term://~/Programming/Eggsh//253233:/usr/bin/bash
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 11 - ((6 * winheight(0) + 6) / 13)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 11
normal! 0
lcd ~/Programming/Eggsh
wincmd w
argglobal
if bufexists(fnamemodify("~/Programming/Eggsh/Makefile", ":p")) | buffer ~/Programming/Eggsh/Makefile | else | edit ~/Programming/Eggsh/Makefile | endif
if &buftype ==# 'terminal'
  silent file ~/Programming/Eggsh/Makefile
endif
balt ~/Programming/Eggsh/todo.md
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 12) / 24)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
lcd ~/Programming/Eggsh
wincmd w
argglobal
if bufexists(fnamemodify("term://~/Programming/Eggsh//253233:/usr/bin/bash", ":p")) | buffer term://~/Programming/Eggsh//253233:/usr/bin/bash | else | edit term://~/Programming/Eggsh//253233:/usr/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term://~/Programming/Eggsh//253233:/usr/bin/bash
endif
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
let s:l = 46 - ((14 * winheight(0) + 7) / 15)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 46
normal! 03|
lcd ~/Programming/Eggsh
wincmd w
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe '2resize ' . ((&lines * 13 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 118 + 119) / 238)
exe '3resize ' . ((&lines * 24 + 28) / 56)
exe 'vert 3resize ' . ((&columns * 118 + 119) / 238)
exe '4resize ' . ((&lines * 15 + 28) / 56)
exe 'vert 4resize ' . ((&columns * 118 + 119) / 238)
tabnext
edit ~/Programming/Eggsh/include/tokenizer.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 16 + 28) / 56)
exe '2resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
exe '3resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 3resize ' . ((&columns * 118 + 119) / 238)
argglobal
balt ~/Programming/Eggsh/src/command_builder_and_tokenizer.c
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 8) / 16)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("~/Programming/Eggsh/src/command_builder_and_tokenizer.c", ":p")) | buffer ~/Programming/Eggsh/src/command_builder_and_tokenizer.c | else | edit ~/Programming/Eggsh/src/command_builder_and_tokenizer.c | endif
if &buftype ==# 'terminal'
  silent file ~/Programming/Eggsh/src/command_builder_and_tokenizer.c
endif
balt ~/Programming/Eggsh/main.c
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 85 - ((8 * winheight(0) + 18) / 36)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 85
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("term://~/Programming/Eggsh//253233:/usr/bin/bash", ":p")) | buffer term://~/Programming/Eggsh//253233:/usr/bin/bash | else | edit term://~/Programming/Eggsh//253233:/usr/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term://~/Programming/Eggsh//253233:/usr/bin/bash
endif
balt ~/Programming/Eggsh/src/command_builder_and_tokenizer.c
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
let s:l = 46 - ((35 * winheight(0) + 18) / 36)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 46
normal! 0
wincmd w
2wincmd w
exe '1resize ' . ((&lines * 16 + 28) / 56)
exe '2resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
exe '3resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 3resize ' . ((&columns * 118 + 119) / 238)
tabnext 2
set stal=1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
