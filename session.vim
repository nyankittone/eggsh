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
badd +50 main.c
badd +1637 term://~/Programming/Eggsh//253233:/usr/bin/bash
badd +6 todo.md
badd +63 Makefile
badd +215 src/command_builder_and_tokenizer.c
badd +57 include/tokenizer.h
badd +4 version_config.mk
badd +251 man://git-branch(1)
badd +29 man://git-reset(1)
badd +26 term://~/Programming/Eggsh//346078:/usr/bin/bash
badd +4 ~/.config/emoji
badd +39 term://~/Programming/Eggsh//346154:/usr/bin/bash
badd +1 ~/.config/clangd/config.yaml
badd +27 term://~/.config/clangd//347740:/usr/bin/bash
badd +3 /tmp/idc.c
badd +5 .clang
badd +0 .gitignore_hidden_message
badd +5 .clangd
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
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe '2resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 118 + 119) / 238)
exe '3resize ' . ((&lines * 16 + 28) / 56)
exe 'vert 3resize ' . ((&columns * 118 + 119) / 238)
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
let s:l = 50 - ((33 * winheight(0) + 26) / 53)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 50
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("Makefile", ":p")) | buffer Makefile | else | edit Makefile | endif
if &buftype ==# 'terminal'
  silent file Makefile
endif
balt version_config.mk
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
let s:l = 120 - ((33 * winheight(0) + 18) / 36)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 120
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
let s:l = 1637 - ((15 * winheight(0) + 8) / 16)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1637
normal! 0
lcd ~/Programming/Eggsh
wincmd w
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe '2resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 118 + 119) / 238)
exe '3resize ' . ((&lines * 16 + 28) / 56)
exe 'vert 3resize ' . ((&columns * 118 + 119) / 238)
tabnext
edit ~/Programming/Eggsh/include/tokenizer.h
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
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe '2resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
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
let s:l = 64 - ((8 * winheight(0) + 8) / 16)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 64
normal! 0
lcd ~/Programming/Eggsh
wincmd w
argglobal
if bufexists(fnamemodify("~/Programming/Eggsh/src/command_builder_and_tokenizer.c", ":p")) | buffer ~/Programming/Eggsh/src/command_builder_and_tokenizer.c | else | edit ~/Programming/Eggsh/src/command_builder_and_tokenizer.c | endif
if &buftype ==# 'terminal'
  silent file ~/Programming/Eggsh/src/command_builder_and_tokenizer.c
endif
balt ~/Programming/Eggsh/.clang
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
let s:l = 215 - ((24 * winheight(0) + 18) / 36)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 215
normal! 028|
lcd ~/Programming/Eggsh
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
let s:l = 1637 - ((52 * winheight(0) + 26) / 53)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1637
normal! 03|
lcd ~/Programming/Eggsh
wincmd w
3wincmd w
exe '1resize ' . ((&lines * 16 + 28) / 56)
exe 'vert 1resize ' . ((&columns * 119 + 119) / 238)
exe '2resize ' . ((&lines * 36 + 28) / 56)
exe 'vert 2resize ' . ((&columns * 119 + 119) / 238)
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
