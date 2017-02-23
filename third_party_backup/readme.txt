============================================================================================
注意，安装代码的时候不要使用Eclipse拷贝

============================================================================================

libelf 库

项目主页：不详
项目链接：http://www.mr511.de/software/

下载：http://www.mr511.de/software/libelf-0.8.13.tar.gz
下载日期：20150418
备份：cp libelf-0.8.13.tar.gz ~/uter/third_party_backup/
整理：tar -zvxf libelf-0.8.13.tar.gz 
             cp -r libelf-0.8.13 ~/uter/third_party_backup/
安装：rm -rf  ~/libelf-0.8.13/
            cd libelf-0.8.13/
            ./configure --prefix=$HOME/libelf-0.8.13
            make
            make install
            ls ~/libelf-0.8.13/
            cp -r ~/libelf-0.8.13  ~/uter/third_party_build/

改造：修改 third_party_build/libelf-0.8.13/include/libelf/libelf.h
            增加:
            //Just for check libelf head file using
            #define UTER_LIBELF_FLAG  (0x44444444)

============================================================================================
libdwarf 库 更新

该版本还因为macor打过补丁

项目主页：http://libdwarf.sourceforge.net/
项目链接：http://sourceforge.net/projects/libdwarf/

下载：git clone git://git.code.sf.net/p/libdwarf/code libdwarf-code
下载日期：20150605
备份：tar -zcvf libdwarf-code_20150417.tar.gz libdwarf-code/
             cp libdwarf-code_20150417.tar.gz ~/uter/third_party_backup/
整理：rm -rf libdwarf-code/.git
             rm -rf libdwarf-code/.gitignore 
             cp -r libdwarf-code ~/uter/third_party_backup/
            修改文件 ~/uter/third_party_backup/libdwarf-code/libdwarf/dwarf_elf_access.c
            增加  int uter_libelf_flag = UTER_LIBELF_FLAG;
安装：cd libdwarf-code/
            export CFLAGS="-I$HOME/uter/third_party_build/libelf-0.8.13/include/libelf/ -I$HOME/uter/third_party_build/libelf-0.8.13/include/"
             ./configure --enable-shared
            make
            ls libdwarf/libdwarf*
            
            rm -rf ~/libdwarf_20150604
            mkdir ~/libdwarf_20150604
            mkdir ~/libdwarf_20150604/include
            mkdir ~/libdwarf_20150604/include/libdwarf
            mkdir ~/libdwarf_20150604/lib
            mkdir ~/libdwarf_20150604/bin
            cp libdwarf/dwarf.h ~/libdwarf_20150604/include/libdwarf/
            cp libdwarf/libdwarf.h ~/libdwarf_20150604/include/libdwarf/
            cp libdwarf/libdwarfdefs.h ~/libdwarf_20150604/include/libdwarf/
            cp libdwarf/libdwarf.a ~/libdwarf_20150604/lib/
            cp libdwarf/libdwarf.so ~/libdwarf_20150604/lib/
            cp dwarfdump/dwarfdump ~/libdwarf_20150604/bin/
            
            cp -r ~/libdwarf_20150604  ~/uter/third_party_build/
            
改造 third_party_build/libdwarf_20150604/include/libdwarf/libdwarf.h
            增加:
            //Just for check libdwarf head file using
            #define UTER_LIBDWARF_FLAG  (0x44444444)

============================================================================================
libdwarf 库

项目主页：http://libdwarf.sourceforge.net/
项目链接：http://sourceforge.net/projects/libdwarf/

下载：git clone git://git.code.sf.net/p/libdwarf/code libdwarf-code
下载日期：20150417
备份：tar -zcvf libdwarf-code_20150417.tar.gz libdwarf-code/
             cp libdwarf-code_20150417.tar.gz ~/uter/third_party_backup/
整理：rm -rf libdwarf-code/.git
             rm -rf libdwarf-code/.gitignore 
             cp -r libdwarf-code ~/uter/third_party_backup/
            修改文件 ~/uter/third_party_backup/libdwarf-code/libdwarf/dwarf_elf_access.c
            增加  int uter_libelf_flag = UTER_LIBELF_FLAG;
安装：cd libdwarf-code/
            export CFLAGS="-I$HOME/uter/third_party_build/libelf-0.8.13/include/libelf/ -I$HOME/uter/third_party_build/libelf-0.8.13/include/"
             ./configure --enable-shared
            make
            ls libdwarf/libdwarf*
            
            rm -rf ~/libdwarf_20150417
            mkdir ~/libdwarf_20150417
            mkdir ~/libdwarf_20150417/include
            mkdir ~/libdwarf_20150417/include/libdwarf
            mkdir ~/libdwarf_20150417/lib
            mkdir ~/libdwarf_20150417/bin
            cp libdwarf/dwarf.h ~/libdwarf_20150417/include/libdwarf/
            cp libdwarf/libdwarf.h ~/libdwarf_20150417/include/libdwarf/
            cp libdwarf/libdwarfdefs.h ~/libdwarf_20150417/include/libdwarf/
            cp libdwarf/libdwarf.a ~/libdwarf_20150417/lib/
            cp libdwarf/libdwarf.so ~/libdwarf_20150417/lib/
            cp dwarfdump/dwarfdump ~/libdwarf_20150417/bin/
            
            cp -r ~/libdwarf_20150417  ~/uter/third_party_build/
            
改造 third_party_build/libdwarf_20150417/include/libdwarf/libdwarf.h
            增加:
            //Just for check libdwarf head file using
            #define UTER_LIBDWARF_FLAG  (0x44444444)
            
============================================================================================
python 库

项目主页：https://www.python.org/
项目连接：https://www.python.org/

下载：https://www.python.org/ftp/python/2.7.9/Python-2.7.9.tgz
下载日期：20150420
备份：cp Python-2.7.9.tgz ~/uter/third_party_backup/
整理：tar -zvxf Python-2.7.9.tgz
             cp -rf Python-2.7.9 ~/uter/third_party_backup/
安装：cd Python-2.7.9/
             rm -rf ~/Python-2.7.9/
             ./configure --prefix=$HOME/Python-2.7.9 --enable-shared
             make
             make install
             ls ~/Python-2.7.9/
             cp -r ~/Python-2.7.9/ ~/uter/third_party_build/
改造：third_party_build/Python-2.7.9/include/python2.7/Python.h
            增加:
            //Just for check libpython head file using
            #define UTER_LIBPYTHON_FLAG  (0x44444444)

============================================================================================
Qt 库：

注:Qt 库很大，涉及内容也超多，而且不是核心库，因此尽量使用二进制库。目前Qt提供的发布方式不是发布压缩表，而是可
执行文件，安装以后会产生一大堆东西，包括 头文件按，库文件，Qt Creater等，并不是我们想要的，需要进行过滤。

项目主页：http://www.qt.io/
项目链接：http://www.qt.io/download-open-source/

下载：http://download.qt.io/official_releases/qt/5.4/5.4.1/qt-opensource-linux-x86-5.4.1.run
             http://download.qt.io/official_releases/qt/5.5/5.5.0/qt-opensource-linux-x86-5.5.0.run
下载日期：20150705
备份： NA
安装：安装 qt-opensource-linux-x86-5.5.0.run
             安装目录：/home/yaozhongkai/Qt5.5.0
             mkdir -p ~/uter/third_party_build/Qt5.5.0
	         cp -r ~/Qt5.5.0/5.5/gcc/include/  ~/uter/third_party_build/Qt5.5.0/
             cp -r ~/Qt5.5.0/5.5/gcc/lib/  ~/uter/third_party_build/Qt5.5.0/
             cp -r ~/Qt5.5.0/5.5/gcc/bin/  ~/uter/third_party_build/Qt5.5.0/
             cp -r ~/Qt5.5.0/5.5/gcc/mkspecs/  ~/uter/third_party_build/Qt5.5.0/
             cp -r ~/Qt5.5.0/5.5/gcc/plugins/  ~/uter/third_party_build/Qt5.5.0/
整理：NA
改造：NA

============================================================================================
Binutils库：

项目主页：http://www.sourceware.org/
项目链接：http://www.sourceware.org/binutils/

下载：http://ftp.gnu.org/gnu/binutils/binutils-2.25.tar.gz
下载日期：20150421
备份：cp binutils-2.25.tar.gz ~/uter/third_party_backup/
整理：tar -zvxf binutils-2.25.tar.gz
             cp -r binutils-2.25 ~/uter/third_party_backup/
安装：cd binutils-2.25/
             rm -rf ~/binutils-2.25_x86
             ./configure --prefix=$HOME/binutils-2.25_x86  --disable-nls --enable-shared --enable-install-libiberty --build=i686-pc-linux-gnu --host=i686-pc-linux-gnu --target=i686-pc-linux-gnu
             make
             make install
             ls ~/binutils-2.25_x86/
             cp -r ~/binutils-2.25_x86 ~/uter/third_party_build/

             cd binutils-2.25/
             rm -rf ~/binutils-2.25_x64
             ./configure --prefix=$HOME/binutils-2.25_x64  --disable-nls --enable-shared --enable-install-libiberty --build=i686-pc-linux-gnu --host=i686-pc-linux-gnu --target=x86_64-pc-linux-gnu
             make
             make install
             cp -r ~/binutils-2.25_x64 ~/uter/third_party_build/
            
============================================================================================
QScintilla库：

项目主页：http://www.riverbankcomputing.co.uk/software/qscintilla/introsourceware.org/
项目链接：http://www.riverbankcomputing.co.uk/software/qscintilla/download/tils/

下载：http://sourceforge.net/projects/pyqt/files/QScintilla2/QScintilla-2.9/QScintilla-gpl-2.9.tar.gz
下载日期：20150803
备份：cp QScintilla-gpl-2.9.tar.gz ~/uter/third_party_backup/
整理：  tar -zvxf QScintilla-gpl-2.9.tar.gz 
               cp -r QScintilla-gpl-2.9 ~/uter/third_party_backup/
安装：
		cd Qt4Qt5/
		export PATH=/home/yaozhongkai/uter/third_party_build/Qt5.5.0/bin/:$PATH
		qmake qscintilla.pro 
 		make
		make install

============================================================================================

首先安装这个：
qt-opensource-windows-x86-mingw492-5.5.0.exe

从代码库中找到这个：
QScintilla-gpl-2.9.tar.gz

然后解压：
使用Qtcreater工具打开(用开始菜单里边的打开文件打开)
...Qt\QScintilla-gpl-2.9\Qt4Qt5\qscintilla.pro

构建，然后等待

右下角的进度条点击可以打开编译窗口

Qt\QScintilla-gpl-2.9\build-qscintilla-Desktop_Qt_5_5_0_MinGW_32bit-Debug\release
里边有输出，.a  .dll

============================================================================================

# Define SciTE settings for Assembler files (NASM)
# Originally by Black Horus blackhorus@gmx.net 2002
# Updated by Kein-Hong Man mkh@pl.jaring.my 2003-10
# Updated by Marat Dukhan (mdukhan3.at.gatech.dot.edu) 10/4/2011

# Nasm files
file.patterns.asm=*.asm
#file.patterns.as=*.s

*source.patterns.asm=$(file.patterns.asm);

filter.asm=Assembler x86/x64 (asm)|$(file.patterns.asm)|
filter.as=Assembler x86/x64 (as)|$(file.patterns.as)|

*filter.asm=$(filter.asm)

lexer.$(file.patterns.asm)=asm
lexer.$(file.patterns.as)=as

*language.assembler=Assembler|asm||

bcd_instruction=aaa aad aam aas daa das
flow_instruction=ja jae jb jbe jc jcxz je jg jge jl jle jmp jna \
	jnae jnb jnbe jnc jne jng jnge jnl jnle jno jnp jns jnz \
	jo jp jpe jpo js jz \
	jcxz jecxz jrcxz \
	loop loope loopne loopz loopnz \
	call ret

alu_instruction=add sub adc sbb neg cmp inc dec \
	and or xor not test \
	shl shr sal sar shld shrd rol ror rcl rcr \
	cbw cwd cwde cdq cdqe cqo \
	bsf bsr bt btc btr bts \
	idiv imul div mul bswap \
	nop

mem_instruction=lea \
	mov movsx movsxd movzx \
	xlatb bound \
	xchg xadd cmpxchg cmpxchg8b cmpxchg16b

stack_instruction=push pop pushad popad pushf popf pushfd popfd pushfq popfq

pred_instruction=seta setae setb setbe setc sete setg setge setl \
	setle setna setnae setnb setnbe setnc setne setng setnge \
	setnl setnle setno setnp setns setnz seto setp setpe setpo \
	sets setz \
	salc

flag_instruction=clc cld stc std cmc lahf sahf

cmov_instruction=cmovo cmovno cmovb cmovc cmovnae cmovae cmovnb \
	cmovnc cmove cmovz cmovne cmovnz cmovbe cmovna cmova \
	cmovnbe cmovs cmovns cmovp cmovpe cmovnp cmovpo cmovl \
	cmovnge cmovge cmovnl cmovle cmovng cmovg cmovnle

prefix_instruction=lock rep repe repz repne repnz

string_instruction=cmpsb cmpsw cmpsq \
	movsb movsw movsq \
	scasb scasw scasd scasq \
	stosb stosw stosd stosq

info_instruction=cpuid rdtsc rdtscp rdpmc xgetbv

lwp_instruction=llwpcb slwpcb lwpval lwpins

ext_instruction=crc32 popcnt lzcnt tzcnt movbe pclmulqdq rdrand

bmi1_instruction=andn bextr blsi blsmk blsr

bmi2_instruction=bzhi mulx pdep pext rorx sarx shlx shrx

tbm_instruction=blcfill blci blcic blcmsk blcs blsfill blsic t1mskc tzmsk

fpu_instruction=f2xm1 fabs fadd faddp fbld fbstp fchs fclex fcom fcomp fcompp fdecstp \
	fdisi fdiv fdivp fdivr fdivrp feni ffree fiadd ficom ficomp fidiv \
	fidivr fild fimul fincstp finit fist fistp fisub fisubr fld fld1 \
	fldcw fldenv fldenvw fldl2e fldl2t fldlg2 fldln2 fldpi fldz fmul \
	fmulp fnclex fndisi fneni fninit fnop fnsave fnsavew fnstcw fnstenv \
	fnstenvw fnstsw fpatan fprem fptan frndint frstor frstorw fsave \
	fsavew fscale fsqrt fst fstcw fstenv fstenvw fstp fstsw fsub fsubp \
	fsubr fsubrp ftst fwait fxam fxch fxtract fyl2x fyl2xp1 \
	fsetpm fcos fldenvd fnsaved fnstenvd fprem1 frstord fsaved fsin fsincos \
	fstenvd fucom fucomp fucompp fcomi fcomip fucomi fucomip ffreep \
	fcmovb fcmove fcmovbe fcmovu fcmovnb fcmovne fcmovnbe fcmovnu

mmx_instruction=movd movq \
	paddb paddw paddd paddsb paddsw paddusb paddusw \
	psubb psubw psubd psubsb psubsw psubusb psubusw \
	pand pandn por pxor \
	pcmpeqb pcmpeqd pcmpeqw pcmpgtb pcmpgtd pcmpgtw \
	pmaddwd pmulhw pmullw \
	psllw pslld psllq psrlw psrld psrlq psraw psrad \
	packuswb packsswb packssdw \
	punpcklbw punpcklwd punpckldq punpckhbw punpckhwd punpckhdq \
	emms

mmx2_instruction=pavgb pavgw \
	pextrw pinsrw pmovmskb \
	pmaxsw pmaxub pminsw pminub \
	pmulhuw psadbw pshufw \
	prefetchnta prefetcht0 prefetcht1 prefetcht2 \
	maskmovq movntq sfence

emmx_instruction=paddsiw psubsiw \
	pmulhrw pmachriw pmulhriw \
	pmagw pdistib paveb \
	pmvzb pmvnzb pmvlzb pmvgezb

k3d_instruction=pfacc pfadd pfsub pfsubr pfmul \
	pfcmpeq pfcmpge pfcmpgt \
	pfmax pfmin \
	pfrcp pfrcpit1 pfrcpit2 pfrsqit1 pfrsqrt \
	pi2fd pf2id \
	pavgusb pmulhrw \
	femms

k3d2_instruction=pfnacc pfpnacc pi2fw pf2iw pswapd

k3dgeode_instruction=pfrsqrtv pfrcpv

k3dprefetch_instruction=prefetch prefetchw

sse_instruction=addss addps subss subps \
	mulss mulps divss divps sqrtss sqrtps \
	rcpss rcpps rsqrtss rsqrtps \
	maxss maxps minss minps \
	cmpss comiss ucomiss cmpps \
	cmpeqss cmpltss cmpless cmpunordss cmpneqss cmpnltss cmpnless cmpordss \
	cmpeqps cmpltps cmpleps cmpunordps cmpneqps cmpnltps cmpnleps cmpordps \
	andnps andps orps xorps \
	cvtsi2ss cvtss2si cvttss2si \
	cvtpi2ps cvtps2pi cvttps2pi \
	movss movlps movhps movlhps movhlps movaps movups movntps movmskps \
	shufps unpckhps unpcklps \
	ldmxcsr stmxcsr

sse2_instruction=addpd addsd subpd subsd \
	mulsd mulpd divsd divpd sqrtsd sqrtpd \
	maxsd maxpd minsd minpd \
	cmpsd comisd ucomisd cmppd \
	cmpeqsd cmpltsd cmplesd cmpunordsd cmpneqsd cmpnltsd cmpnlesd cmpordsd \
	cmpeqpd cmpltpd cmplepd cmpunordpd cmpneqpd cmpnltpd cmpnlepd cmpordpd \
	andnpd andpd orpd xorpd \
	cvtsd2ss cvtpd2ps cvtss2sd cvtps2pd \
	cvtdq2ps cvtps2dq cvttps2dq \
	cvtdq2pd cvtpd2dq cvttpd2dq \
	cvtsi2sd cvtsd2si cvttsd2si \
	cvtpi2pd cvtpd2pi cvttpd2pi \
	movsd movlpd movhpd movapd movupd movntpd movmskpd \
	shufpd unpckhpd unpcklpd \
	movnti movdqa movdqu movntdq maskmovdqu \
	movdq2q movq2dq \
	paddq psubq pmuludq \
	pslldq psrldq \
	punpcklqdq punpckhqdq \
	pshufhw pshuflw pshufd \
	lfence mfence

sse3_instruction=addsubps addsubpd \
	haddps haddpd hsubps hsubpd \
	movsldup movshdup movddup \
	lddqu \
	fisttp

ssse3_instruction=psignb psignw psignd \
	pabsb pabsw pabsd \
	palignr pshufb \
	pmulhrsw pmaddubsw \
	phaddw phaddd phaddsw \
	phsubw phsubd phsubsw

sse4a_instruction=extrq insertq \
	movntsd movntss

sse41_instruction=mpsadbw phminposuw \
	pmuldq pmulld \
	dpps dppd \
	blendps blendpd blendvps blendvpd pblendvb pblendw \
	pmaxsb pmaxuw pmaxsd pmaxud pminsb pminuw pminsd pminud \
	roundps roundss roundpd roundsd \
	insertps pinsrb pinsrd pinsrq \
	extractps pextrb pextrd pextrq \
	pmovsxbw pmovsxbd pmovsxbq pmovsxwd pmovsxwq pmovsxdq \
	pmovzxbw pmovzxbd pmovzxbq pmovzxwd pmovzxwq pmovzxdq \
	ptest \
	pcmpeqq \
	packusdw \
	movntdqa

sse42_instruction=pcmpgtq \
	pcmpestri pcmpestrm pcmpistri pcmpistrm

aes_instruction=aesenc aesenclast aesdec aesdeclast aeskeygenassist aesimc

padlock_instruction=xcryptcbc xcryptcfb xcryptctr xcryptecb xcryptofb \
	xsha1 xsha256 montmul xstore


avx_instruction=vaddss vaddps vaddsd vaddpd vsubss vsubps vsubsd vsubpd \
	vaddsubps vaddsubpd \
	vhaddps vhaddpd vhsubps vhsubpd \
	vmulss vmulps vmulsd vmulpd \
	vmaxss vmaxps vmaxsd vmaxpd vminss vminps vminsd vminpd \
	vandps vandpd vandnps vandnpd vorps vorpd vxorps vxorpd \
	vblendps vblendpd vblendvps vblendvpd \
	vcmpss vcomiss vucomiss vcmpsd vcomisd vucomisd vcmpps vcmppd \
	vcmpeqss vcmpltss vcmpless vcmpunordss vcmpneqss vcmpnltss vcmpnless vcmpordss \
	vcmpeq_uqss vcmpngess vcmpngtss vcmpfalsess vcmpneq_oqss vcmpgess vcmpgtss vcmptruess \
	vcmpeq_osss vcmplt_oqss vcmple_oqss vcmpunord_sss vcmpneq_usss vcmpnlt_uqss vcmpnle_uqss vcmpord_sss \
	vcmpeq_usss vcmpnge_uqss vcmpngt_uqss vcmpfalse_osss vcmpneq_osss vcmpge_oqss vcmpgt_oqss vcmptrue_usss \
	vcmpeqps vcmpltps vcmpleps vcmpunordps vcmpneqps vcmpnltps vcmpnleps vcmpordps \
	vcmpeq_uqps vcmpngeps vcmpngtps vcmpfalseps vcmpneq_oqps vcmpgeps vcmpgtps vcmptrueps \
	vcmpeq_osps vcmplt_oqps vcmple_oqps vcmpunord_sps vcmpneq_usps vcmpnlt_uqps vcmpnle_uqps vcmpord_sps \
	vcmpeq_usps vcmpnge_uqps vcmpngt_uqps vcmpfalse_osps vcmpneq_osps vcmpge_oqps vcmpgt_oqps vcmptrue_usps \
	vcmpeqsd vcmpltsd vcmplesd vcmpunordsd vcmpneqsd vcmpnltsd vcmpnlesd vcmpordsd \
	vcmpeq_uqsd vcmpngesd vcmpngtsd vcmpfalsesd vcmpneq_oqsd vcmpgesd vcmpgtsd vcmptruesd \
	vcmpeq_ossd vcmplt_oqsd vcmple_oqsd vcmpunord_ssd vcmpneq_ussd vcmpnlt_uqsd vcmpnle_uqsd vcmpord_ssd \
	vcmpeq_ussd vcmpnge_uqsd vcmpngt_uqsd vcmpfalse_ossd vcmpneq_ossd vcmpge_oqsd vcmpgt_oqsd vcmptrue_ussd \
	vcmpeqpd vcmpltpd vcmplepd vcmpunordpd vcmpneqpd vcmpnltpd vcmpnlepd vcmpordpd \
	vcmpeq_uqpd vcmpngepd vcmpngtpd vcmpfalsepd vcmpneq_oqpd vcmpgepd vcmpgtpd vcmptruepd \
	vcmpeq_ospd vcmplt_oqpd vcmple_oqpd vcmpunord_spd vcmpneq_uspd vcmpnlt_uqpd vcmpnle_uqpd vcmpord_spd \
	vcmpeq_uspd vcmpnge_uqpd vcmpngt_uqpd vcmpfalse_ospd vcmpneq_ospd vcmpge_oqpd vcmpgt_oqpd vcmptrue_uspd \
	vcvtsd2ss vcvtpd2ps vcvtss2sd vcvtps2pd \
	vcvtsi2ss vcvtss2si vcvttss2si \
	vcvtpi2ps vcvtps2pi vcvttps2pi \
	vcvtdq2ps vcvtps2dq vcvttps2dq \
	vcvtdq2pd vcvtpd2dq vcvttpd2dq \
	vcvtsi2sd vcvtsd2si vcvttsd2si \
	vcvtpi2pd vcvtpd2pi vcvttpd2pi \
	vdivss vdivps vdivsd vdivpd vsqrtss vsqrtps vsqrtsd vsqrtpd \
	vdpps vdppd \
	vmaskmovps vmaskmovpd \
	vmovss vmovsd vmovaps vmovapd vmovups vmovupd vmovntps vmovntpd \
	vmovhlps vmovlhps vmovlps vmovlpd vmovhps vmovhpd \
	vmovsldup vmovshdup vmovddup \
	vmovmskps vmovmskpd \
	vroundss vroundps vroundsd vroundpd \
	vrcpss vrcpps vrsqrtss vrsqrtps \
	vunpcklps vunpckhps vunpcklpd vunpckhpd \
	vbroadcastss vbroadcastsd vbroadcastf128 \
	vextractps vinsertps vextractf128 vinsertf128 \
	vshufps vshufpd vpermilps vpermilpd vperm2f128 \
	vtestps vtestpd \
	vpaddb vpaddusb vpaddsb vpaddw vpaddusw vpaddsw vpaddd vpaddq \
	vpsubb vpsubusb vpsubsb vpsubw vpsubusw vpsubsw vpsubd vpsubq \
	vphaddw vphaddsw vphaddd vphsubw vphsubsw vphsubd \
	vpsllw vpslld vpsllq vpsrlw vpsrld vpsrlq vpsraw vpsrad \
	vpand vpandn vpor vpxor \
	vpblendwb vpblendw \
	vpsignb vpsignw vpsignd \
	vpavgb vpavgw \
	vpabsb vpabsw vpabsd \
	vmovd vmovq vmovdqa vmovdqu vlddqu vmovntdq vmovntdqa vmaskmovdqu \
	vpmovsxbw vpmovsxbd vpmovsxbq vpmovsxwd vpmovsxwq vpmovsxdq \
	vpmovzxbw vpmovzxbd vpmovzxbq vpmovzxwd vpmovzxwq vpmovzxdq \
	vpackuswb vpacksswb vpackusdw vpackssdw \
	vpcmpeqb vpcmpeqw vpcmpeqd vpcmpeqq vpcmpgtb vpcmpgtw vpcmpgtd vpcmpgtq \
	vpmaddubsw vpmaddwd \
	vpmullw vpmulhuw vpmulhw vpmulhrsw vpmulld vpmuludq vpmuldq \
	vpmaxub vpmaxsb vpmaxuw vpmaxsw vpmaxud vpmaxsd \
	vpminub vpminsb vpminuw vpminsw vpminud vpminsd \
	vpmovmskb vptest \
	vpunpcklbw vpunpcklwd vpunpckldq vpunpcklqdq \
	vpunpckhbw vpunpckhwd vpunpckhdq vpunpckhqdq \
	vpslldq vpsrldq vpalignr \
	vpshufb vpshuflw vpshufhw vpshufd \
	vpextrb vpextrw vpextrd vpextrq vpinsrb vpinsrw vpinsrd vpinsrq \
	vpsadbw vmpsadbw vphminposuw \
	vpcmpestri vpcmpestrm vpcmpistri vpcmpistrm \
	vpclmulqdq vaesenc vaesenclast vaesdec vaesdeclast vaeskeygenassist vaesimc \
	vldmxcsr vstmxcsr vzeroall vzeroupper

avx2_instruction=vbroadcasti128 vpbroadcastb vpbroadcastw vpbroadcastd vpbroadcastq \
	vpblendd \
	vpermd vpermq vperm2i128 \
	vextracti128 vinserti128 \
	vpmaskmovd vpmaskmovq \
	vpsllvd vpsllvq vpsravd vpsrlvd vpsrldq \
	vpgatherdd vpgatherqd vgatherdq vgatherqq \
	vpermps vpermpd \
	vgatherdpd vgatherqpd vgatherdps vgatherqps

xop_instruction=vfrczss vfrczps vfrczsd vfrczpd \
	vpermil2ps vperlil2pd \
	vtestps vtestpd \
	vpcomub vpcomb vpcomuw vpcomw vpcomud vpcomd vpcomuq vpcomq \
	vphaddubw vphaddbw vphaddubd vphaddbd vphaddubq vphaddbq \
	vphadduwd vphaddwd vphadduwq vphaddwq vphaddudq vphadddq \
	vphsubbw vphsubwd vphsubdq \
	vpmacsdd vpmacssdd vpmacsdql vpmacssdql vpmacsdqh vpmacssdqh \
	vpmacsww vpmacssww vpmacswd vpmacsswd \
	vpmadcswd vpmadcsswd \
	vpcmov vpperm \
	vprotb vprotw vprotd vprotq \
	vpshab vpshaw vpshad vpshaq \
	vpshlb vpshlw vpshld vpshlq

cvt16_instruction=vcvtph2ps vcvtps2ph

fma4_instruction=vfmaddss vfmaddps vfmaddsd vfmaddpd \
	vfmsubss vfmsubps vfmsubsd vfmsubpd \
	vnfmaddss vnfmaddps vnfmaddsd vnfmaddpd \
	vnfmsubss vnfmsubps vnfmsubsd vnfmsubpd \
	vfmaddsubps vfmaddsubpd vfmsubaddps vfmsubaddpd

fma3_instruction=vfmadd132ss vfmadd213ss vfmadd231ss \
	vfmadd132ps vfmadd213ps vfmadd231ps \
	vfmadd132sd vfmadd213sd vfmadd231sd \
	vfmadd132pd vfmadd213pd vfmadd231pd \
	vfmaddsub132ps vfmaddsub213ps vfmaddsub231ps \
	vfmaddsub132pd vfmaddsub213pd vfmaddsub231pd \
	vfmsubadd132ps vfmsubadd213ps vfmsubadd231ps \
	vfmsubadd132pd vfmsubadd213pd vfmsubadd231pd \
	vfmsub132ss vfmsub213ss vfmsub231ss \
	vfmsub132ps vfmsub213ps vfmsub231ps \
	vfmsub132sd vfmsub213sd vfmsub231sd \
	vfmsub132pd vfmsub213pd vfmsub231pd \
	vfnmadd132ss vfnmadd213ss vfnmadd231ss \
	vfnmadd132ps vfnmadd213ps vfnmadd231ps \
	vfnmadd132sd vfnmadd213sd vfnmadd231sd \
	vfnmadd132pd vfnmadd213pd vfnmadd231pd \
	vfnmsub132ss vfnmsub213ss vfnmsub231ss \
	vfnmsub132ps vfnmsub213ps vfnmsub231ps \
	vfnmsub132sd vfnmsub213sd vfnmsub231sd \
	vfnmsub132pd vfnmsub213pd vfnmsub231pd

register=al ah bl bh cl ch dl dh \
	ax bx cx dx si di bp \
	eax ebx ecx edx esi edi ebx esp \
	st0 st1 st2 st3 st4 st5 st6 st7 \
	mm0 mm1 mm2 mm3 mm4 mm5 mm6 mm7 \
	xmm0 xmm1 xmm2 xmm3 xmm4 xmm5 xmm6 xmm7 \
	ymm0 ymm1 ymm2 ymm3 ymm4 ymm5 ymm6 ymm7 \
	fs

register_64=sil dil bpl \
	r8b r9b r10b r11b r12b r13b r14b r15b \
	r8w r9w r10w r11w r12w r13w r14w r15w \
	rax rcx rdx rbx rsp rbp rsi rdi \
	r8 r9 r10 r11 r12 r13 r14 r15 \
	xmm8 xmm9 xmm10 xmm11 xmm12 xmm13 xmm14 xmm15 \
	ymm8 ymm9 ymm10 ymm11 ymm12 ymm13 ymm14 ymm15 \
	gs

directive_nasm=db dw dd dq dt do dy \
	resb resw resd resq rest reso resy incbin equ times safeseh \
	__utf16__ __utf32__ %+ \
	default cpu float start imagebase osabi \
	..start ..imagebase ..gotpc ..gotoff ..gottpoff ..got ..plt ..sym ..tlsie \
	section segment __sect__ group absolute \
	.bss .comment .data .lbss .ldata .lrodata .rdata .rodata .tbss .tdata .text \
	alloc bss code exec data noalloc nobits noexec nowrite progbits rdata tls write \
	private public common stack overlay class \
	extern global common import export \
	%define %idefine %xdefine %ixdefine %assign %undef %? %?? \
	%defstr %idefstr %deftok %ideftok \
	%strcat %strlen %substr \
	%macro %imacro %rmacro %exitmacro %endmacro %unmacro \
	%if %ifn %elif %elifn %else %endif \
	%ifdef %ifndef %elifdef %elifndef \
	%ifmacro %ifnmacro %elifmacro %elifnmacro \
	%ifctx %ifnctx %elifctx %elifnctx \
	%ifidn %ifnidn %elifidn %elifnidn \
	%ifidni %ifnidni %elifidni %elifnidni \
	%ifid %ifnid %elifid %elifnid \
	%ifnum %ifnnum %elifnum %elifnnum \
	%ifstr %ifnstr %elifstr %elifnstr \
	%iftoken %ifntoken %eliftoken %elifntoken \
	%ifempty %elifempty %ifnempty %elifnempty \
	%ifenv %ifnenv %elifenv %elifnenv \
	%rep %exitrep %endrep \
	%while %exitwhile %endwhile \
	%include %pathsearch %depend %use \
	%push %pop %repl %arg %local %stacksize flat flat64 large small \
	%error %warning %fatal \
	%00 .nolist %rotate %line %! %final %clear \
	struc endstruc istruc at iend \
	align alignb sectalign \
	bits use16 use32 use64 \
	__nasm_major__ __nasm_minor__ __nasm_subminor__ ___nasm_patchlevel__ \
	__nasm_version_id__ __nasm_ver__ \
	__file__ __line__ __pass__ __bits__ __output_format__ \
	__date__ __time__ __date_num__ __time_num__ __posix_time__ \
	__utc_date__ __utc_time__ __utc_date_num__ __utc_time_num__ \
	__float_daz__ __float_round__ __float__ \
	__use_altreg__ altreg \
	__use_smartalign__ smartalign __alignmode__ \
	__use_fp__ __infinity__ __nan__ __qnan__ __snan__ \
	__float8__ __float16__ __float32__ __float64__ __float80m__ __float80e__ __float128l__ __float128h__

directive_operand_nasm=a16 a32 a64 o16 o32 o64 strict \
	byte word dword qword tword oword yword nosplit \
	%0 %1 %2 %3 %4 %5 %6 %7 %8 %9 \
	abs rel \
	$ $$ \
	seg wrt

keywords.$(file.patterns.asm)=$(bcd_instruction) $(flow_instruction) $(alu_instruction) $(mem_instruction) $(stack_instruction) $(pred_instruction) $(flag_instruction) $(cmov_instruction) $(prefix_instruction) $(string_instruction) $(info_instruction) $(lwp_instruction) $(ext_instruction) $(bmi1_instruction) $(bmi2_instruction)
keywords2.$(file.patterns.asm)=$(fpu_instruction)
keywords3.$(file.patterns.asm)=$(register) $(register_64)
keywords4.$(file.patterns.asm)=$(directive_nasm)
keywords5.$(file.patterns.asm)=$(directive_operand_nasm)
keywords6.$(file.patterns.asm)=$(mmx_instruction) $(mmx2_instruction) $(emmx_instruction) $(k3d_instruction) $(k3d2_instruction) $(k3dgeode_instruction) $(k3dprefetch_instruction) $(sse_instruction) $(sse2_instruction) $(sse3_instruction) $(ssse3_instruction) $(sse4a_instruction) $(sse41_instruction) $(sse42_instruction) $(aes_instruction) $(padlock_instruction) $(avx_instruction) $(avx2_instruction) $(xop_instruction) $(cvt16_instruction) $(fma4_instruction) $(fma3_instruction)

keywords.$(file.patterns.as)=$(bcd_instruction) $(flow_instruction) $(alu_instruction) $(mem_instruction) $(stack_instruction) $(pred_instruction) $(flag_instruction) $(cmov_instruction) $(prefix_instruction) $(string_instruction) $(info_instruction) $(lwp_instruction) $(ext_instruction) $(bmi1_instruction) $(bmi2_instruction)
keywords2.$(file.patterns.as)=$(fpu_instruction)
keywords3.$(file.patterns.as)=$(register) $(register_64)
keywords4.$(file.patterns.as)=$(directive_nasm)
keywords5.$(file.patterns.as)=$(directive_operand_nasm)
keywords6.$(file.patterns.as)=$(mmx_instruction) $(mmx2_instruction) $(emmx_instruction) $(k3d_instruction) $(k3d2_instruction) $(k3dgeode_instruction) $(k3dprefetch_instruction) $(sse_instruction) $(sse2_instruction) $(sse3_instruction) $(ssse3_instruction) $(sse4a_instruction) $(sse41_instruction) $(sse42_instruction) $(aes_instruction) $(padlock_instruction) $(avx_instruction) $(avx2_instruction) $(xop_instruction) $(cvt16_instruction) $(fma4_instruction) $(fma3_instruction)

comment.block.asm=;
comment.block.as=#


# Assembler Styles
#style.asm.32=fore:#808080,font:Verdana,size:8
#style.as.32=$(style.asm.32)
# Default
style.asm.0=
style.asm.0=$(style.asm.0)
# Comment
style.asm.1=fore:#adadad
style.as.1=$(style.asm.1)
#$(colour.code.comment.line),font:Comic Sans MS,size:8
# Number
style.asm.2=fore:#ff0000
style.as.2=$(style.asm.2)
# String
style.asm.3=$(colour.string)
style.as.3=$(style.asm.3)
# Operator
style.asm.4=$(colour.operator),bold
style.as.4=$(style.asm.4)
# Identifier
style.asm.5=
style.as.5=$(style.asm.5)
# CPU instruction
style.asm.6=$(colour.keyword),bold
style.as.6=$(style.asm.6)
#fore:#0000ff
# FPU instruction
style.asm.7=fore:#0000ff
style.as.7=$(style.asm.7)
# Register
style.asm.8=fore:#46aa03,bold
style.as.8=$(style.asm.8)
# assembler Directive
style.asm.9=fore:#0000ff
style.as.9=$(style.asm.9)
# assembler Directive Operand
style.asm.10=fore:#0000ff
style.as.10=$(style.asm.10)
# Comment block (GNU as /*...*/ syntax, unimplemented)
style.asm.11=$(colour.code.comment.box),$(font.code.comment.box)
style.as.11=$(style.asm.11)
# Character/String (single quote) (also character prefix in GNU as)
style.asm.12=$(colour.char)
style.as.12=$(style.asm.12)
# End of line where string is not closed
style.asm.13=fore:#000000,back:#E0C0E0,eolfilled
style.as.13=$(style.asm.13)
# Extended instructions
style.asm.14=fore:#B00040
style.as.14=$(style.asm.14)
# Comment directive
style.asm.15=fore:#66AA00
style.as.15=$(style.asm.15)


command.compile.$(file.patterns.asm)=nasm $(FileNameExt)

command.name.0.*.asm=Link
command.0.*.asm=link $(FileName)


statement.indent.$(file.patterns.asm)=9 .if
statement.indent.$(file.patterns.as)=9 .if


============================================================================================



