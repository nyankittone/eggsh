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
badd +83 main.c
badd +10 todo.md
badd +1 Makefile
badd +325 src/command_builder_and_tokenizer.c
badd +84 include/tokenizer.h
badd +9 include/testing/tokenizer_runner.h
badd +1 tests_src/tokenizer_runner.c
badd +194 src/hash_map.c
badd +97 src/command_runner.c
badd +0 term://~/Programming/Eggsh//478306:/usr/bin/bash
badd +681 man://sigaction(2)
badd +152 man://signal(7)
badd +75 term://~/Programming/Eggsh//523606:/usr/bin/bash
badd +1 ~/Source_Code/dash/src/alias.c
badd +329 ~/Source_Code/dash/src/trap.c
badd +45 ~/Source_Code/dash/src/main.h
badd +53 ~/Source_Code/dash/src/options.h
badd +108 ~/Source_Code/dash/src/options.c
badd +21 man://sigsetops(3)
badd +95 ~/Source_Code/dash/src/jobs.h
badd +61 ~/Source_Code/dash/src/error.h
badd +92 ~/Source_Code/dash/src/error.c
badd +1 man://setjmp(3)
badd +174 ~/Source_Code/dash/src/main.c
badd +94 ~/Source_Code/dash/src/shell.h
badd +22 man://sigqueue(3)
badd +14 man://sigaction(3p)
argglobal
%argdel
$argadd main.c
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit include/testing/tokenizer_runner.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
wincmd _ | wincmd |
split
1wincmd k
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
exe '1resize ' . ((&lines * 14 + 28) / 57)
exe 'vert 1resize ' . ((&columns * 116 + 119) / 238)
exe '2resize ' . ((&lines * 39 + 28) / 57)
exe 'vert 2resize ' . ((&columns * 116 + 119) / 238)
exe '3resize ' . ((&lines * 27 + 28) / 57)
exe 'vert 3resize ' . ((&columns * 121 + 119) / 238)
exe '4resize ' . ((&lines * 26 + 28) / 57)
exe 'vert 4resize ' . ((&columns * 121 + 119) / 238)
argglobal
balt tests_src/tokenizer_runner.c
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
let s:l = 1 - ((0 * winheight(0) + 7) / 14)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 011|
wincmd w
argglobal
if bufexists(fnamemodify("main.c", ":p")) | buffer main.c | else | edit main.c | endif
if &buftype ==# 'terminal'
  silent file main.c
endif
balt include/testing/tokenizer_runner.h
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
let s:l = 71 - ((16 * winheight(0) + 19) / 39)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 71
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("todo.md", ":p")) | buffer todo.md | else | edit todo.md | endif
if &buftype ==# 'terminal'
  silent file todo.md
endif
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
let s:l = 19 - ((18 * winheight(0) + 13) / 27)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 19
normal! 013|
lcd ~/Programming/Eggsh
wincmd w
argglobal
if bufexists(fnamemodify("term://~/Programming/Eggsh//478306:/usr/bin/bash", ":p")) | buffer term://~/Programming/Eggsh//478306:/usr/bin/bash | else | edit term://~/Programming/Eggsh//478306:/usr/bin/bash | endif
if &buftype ==# 'terminal'
  silent file term://~/Programming/Eggsh//478306:/usr/bin/bash
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
let s:l = 9027 - ((25 * winheight(0) + 13) / 26)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 9027
normal! 034|
lcd ~/Programming/Eggsh
wincmd w
2wincmd w
exe '1resize ' . ((&lines * 14 + 28) / 57)
exe 'vert 1resize ' . ((&columns * 116 + 119) / 238)
exe '2resize ' . ((&lines * 39 + 28) / 57)
exe 'vert 2resize ' . ((&columns * 116 + 119) / 238)
exe '3resize ' . ((&lines * 27 + 28) / 57)
exe 'vert 3resize ' . ((&columns * 121 + 119) / 238)
exe '4resize ' . ((&lines * 26 + 28) / 57)
exe 'vert 4resize ' . ((&columns * 121 + 119) / 238)
tabnext
edit ~/Programming/Eggsh/src/command_runner.c
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
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
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
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
let s:l = 97 - ((27 * winheight(0) + 27) / 54)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 97
normal! 021|
wincmd w
argglobal
if bufexists(fnamemodify("~/Programming/Eggsh/src/command_builder_and_tokenizer.c", ":p")) | buffer ~/Programming/Eggsh/src/command_builder_and_tokenizer.c | else | edit ~/Programming/Eggsh/src/command_builder_and_tokenizer.c | endif
if &buftype ==# 'terminal'
  silent file ~/Programming/Eggsh/src/command_builder_and_tokenizer.c
endif
balt ~/Programming/Eggsh/include/tokenizer.h
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
let s:l = 208 - ((19 * winheight(0) + 27) / 54)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 208
normal! 025|
wincmd w
exe 'vert 1resize ' . ((&columns * 118 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
tabnext
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
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
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 118 + 119) / 238)
argglobal
enew
file man://signal(7)
balt term://~/Programming/Eggsh//478306:/usr/bin/bash
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal nofoldenable
lcd ~/Programming/Eggsh
wincmd w
argglobal
enew
file man://sigaction(2)
balt man://sigsetops(3)
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal nofoldenable
lcd ~/Programming/Eggsh
wincmd w
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe 'vert 2resize ' . ((&columns * 118 + 119) / 238)
tabnext 1
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
