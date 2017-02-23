/******************************************************************************
 * Copyright(C),2015-2015, Yaozhongkai
 * FileName:
 * Author:Yaozhongkai
 * Version:
 * Date:
 * Description:
 * Others:
 * Function List:
 * History:
 * ****************************************************************************/

#include <uter_public.h>
#include <target.hxx>
#include <tester.hxx>

/*Jump support i386*/
static int asm_parser(QString &qasmstr, unsigned long long &jmpto)
{
	jmpto = 0;

	if (qasmstr.length() == 0) {
		return -1;
	}

	if (qasmstr.mid(0, 1) != "j") {
		return 0;
	}

	int findat = qasmstr.indexOf("0x");
	if (findat < 0) {
		return 0;
	}

	QString jmptostr = qasmstr.mid(findat + 2);
	if (jmptostr.length() == 0) {
		return 0;
	}

	jmpto = jmptostr.toULongLong(NULL, 16);
	return 1;
}

/*Load token set from asm list*/
int CovTokenSet::loadFromAsmSet(BfdAsmSet *asmset)
{
	UTER_ASSERT(asmset != NULL);
	int result;

	if (asmset->m_bfd_asm_list.empty()) {
		return 0;
	}

	/*Some address on jump to*/
	QSet<unsigned long long> jmp_dst;
	QSet<unsigned long long>::iterator jmp_iter;

	QList<BfdAsm*>::iterator asm_iter;
	QMap<unsigned long long, CovToken*>::iterator token_iter;

	/*Round 1: scan all jump target*/
	for (asm_iter = asmset->m_bfd_asm_list.begin(); asm_iter != asmset->m_bfd_asm_list.end(); asm_iter++) {
		BfdAsm *bfdasm = *asm_iter;
		unsigned long long jmpto;
		result = asm_parser(bfdasm->m_asm_code, jmpto);
		if (result <= 0) {
			/*Ignore error and not jump*/
			continue;
		}
		jmp_dst.insert(jmpto);
	}

	/*Round 2: make all token by jump from and to*/
	for (asm_iter = asmset->m_bfd_asm_list.begin(); asm_iter != asmset->m_bfd_asm_list.end(); asm_iter++) {
		BfdAsm *bfdasm = *asm_iter;
		unsigned long long jmpto;
		result = asm_parser(bfdasm->m_asm_code, jmpto);
		if (result < 0) {
			/*Ignore error*/
			continue;
		} else if (result == 1) {
			/*If current asm is jump, fork after current asm, current is last asm in token, so try to make token*/
			token_iter = m_utertoken_map.find(bfdasm->m_offset);
			if (token_iter == m_utertoken_map.end()) {
				CovToken *token = new CovToken;
				token->m_addr = bfdasm->m_offset;
				m_utertoken_map.insert(token->m_addr, token);
			}
		}

		unsigned long long next = bfdasm->m_offset + bfdasm->m_octets;
		jmp_iter = jmp_dst.find(next);
		if (jmp_iter != jmp_dst.end()) {
			/*If next asm is jump target, merge after current asm, current is last asm in token, so try to make token*/
			token_iter = m_utertoken_map.find(bfdasm->m_offset);
			if (token_iter == m_utertoken_map.end()) {
				CovToken *token = new CovToken;
				token->m_addr = bfdasm->m_offset;
				m_utertoken_map.insert(token->m_addr, token);
			}
		}
	}

	/*Last asm is a token*/
	BfdAsm *bfdasm = asmset->m_bfd_asm_list.back();
	token_iter = m_utertoken_map.find(bfdasm->m_offset);
	if (token_iter == m_utertoken_map.end()) {
		CovToken *token = new CovToken;
		token->m_addr = bfdasm->m_offset;
		m_utertoken_map.insert(token->m_addr, token);
	}

	/*Round 3: associate all asm to token, reverse foreach*/
	CovToken *last_token = NULL;
	for (asm_iter = asmset->m_bfd_asm_list.end(); asm_iter != asmset->m_bfd_asm_list.begin(); /*none*/) {
		BfdAsm *bfdasm = *(--asm_iter);
		/*Token index by last asm*/
		token_iter = m_utertoken_map.find(bfdasm->m_offset);
		if (token_iter != m_utertoken_map.end()) {
			last_token = token_iter.value();
		}
		UTER_ASSERT(last_token != NULL);

		/*address to token*/
		m_addtoken_map.insert(bfdasm->m_offset, last_token);
	}

	return 0;
}

int CovTokenSet::setTokenSet(TargetMod *mod)
{
	UTER_ASSERT(mod != NULL);

	/*For each function*/
	QMap<QString, DwfCu*>::iterator iter;
	for (iter = mod->m_dwarf_cu_map.begin(); iter != mod->m_dwarf_cu_map.end(); iter++) {
		DwfCu *dwfcu = iter.value();
		QList<DwfFun*>::iterator fun_iter;
		for (fun_iter = dwfcu->m_funs.begin(); fun_iter != dwfcu->m_funs.end(); fun_iter++) {
			DwfFun *fun = *fun_iter;
			/*Locate bfd section*/
			BfdSec *on_sec = mod->bfdGetSecByAddrIn(mod->m_add_mkup + fun->m_add_range.m_begin);
			if (on_sec == NULL) {
				UTER_LOG("Can not find section for function %s with address 0x%016llx on module %s.",
						  fun->m_name.toStdString().c_str(), fun->m_add_range.m_begin,
						  mod->m_remote_path.toStdString().c_str());
				return -1;
			}

			/*Disassemble function code*/
			BfdAsmSet *asmset = on_sec->dasm(fun->m_add_range.m_begin,
					                         fun->m_add_range.m_begin + fun->m_add_range.m_offset);
			if (asmset == NULL) {
				UTER_DEBUG("Disassemble function 0x%016llx on module %s failed.",
						  fun->m_add_range.m_begin, mod->m_remote_path.toStdString().c_str());
				return -1;
			}
			int result = fun->m_cov_token.loadFromAsmSet(asmset);
			delete asmset;
			if (result < 0) {
				UTER_DEBUG("Make token information for function 0x%016llx on module %s failed.",
						 fun->m_add_range.m_begin, mod->m_remote_path.toStdString().c_str());
				return -1;
			}
		}
	}

	return 0;
}

FunCovInfo *DwfCu::funCovInfoGet(DwfFun *fun)
{
	UTER_ASSERT(fun != NULL);
	FunCovInfo *ret_info = new FunCovInfo;

	QList<DwfLine*>::iterator iter;
	for (iter = m_dwf_line_list.begin(); iter != m_dwf_line_list.end(); iter++) {
		DwfLine *dwfline = *iter;
		if (dwfline->m_isend) {
			/*Ignore end statement*/
			continue;
		}
		CovToken *token = fun->m_cov_token.getTokenByAddr(dwfline->m_lineaddr);
		if (token == NULL) {
			continue;
		}

		/*Get or make line coverage*/
		FunCovLine *covline;
		QMap<FileLineNo, FunCovLine*>::iterator iter_line;
		iter_line = ret_info->m_cov_line_map.find(FileLineNo(dwfline->m_fileno, dwfline->m_lineno));
		if (iter_line == ret_info->m_cov_line_map.end()) {
			covline = new FunCovLine;
			covline->m_fileline_no.m_file_no = dwfline->m_fileno;
			covline->m_fileline_no.m_line_no = dwfline->m_lineno;
			ret_info->m_cov_line_map.insert(covline->m_fileline_no, covline);
			if (ret_info->m_filename_map.find(dwfline->m_fileno) == ret_info->m_filename_map.end()) {
				/*add file path*/
				ret_info->m_filename_map.insert(dwfline->m_fileno, getFileNameByNo(dwfline->m_fileno));
			}
		} else {
			covline = iter_line.value();
		}

		/*Make statement coverage and set it to line coverage*/
		FunCovStat *covstat = new FunCovStat;
		covstat->m_addr = dwfline->m_lineaddr;
		covstat->m_count_fun = token->m_hit_for_fun;
		covline->m_stat_list.push_back(covstat);
	}

	return ret_info;
}

int DwfCu::funCovOn(DwfFun *fun)
{
	UTER_ASSERT(fun != NULL);

	QMap<unsigned long long, CovToken*>::iterator iter_token;
	for (iter_token = fun->m_cov_token.m_utertoken_map.begin();
		 iter_token != fun->m_cov_token.m_utertoken_map.end(); iter_token++) {
		CovToken *token =  iter_token.value();
		token->m_hit_for_fun = false;
		TargetBreak *brkpnt = g_target->breakGetByAddr(token->m_addr);
		if (brkpnt == NULL || !(brkpnt->m_type_flag & TargetBreak::BREAK_COVFUN)) {
			/*Insert coverage break*/
			g_target->breakInsert(token->m_addr, TargetBreak::BREAK_COVFUN, token);
		}
	}

	return 0;
}

int DwfCu::funCovOff(DwfFun *fun)
{
	UTER_ASSERT(fun != NULL);

	QMap<unsigned long long, CovToken*>::iterator iter_token;
	for (iter_token = fun->m_cov_token.m_utertoken_map.begin();
		 iter_token != fun->m_cov_token.m_utertoken_map.end(); iter_token++) {
		CovToken *token =  iter_token.value();
		token->m_hit_for_fun = false;
		TargetBreak *brkpnt = g_target->breakGetByAddr(token->m_addr);
		if (brkpnt != NULL && (brkpnt->m_type_flag & TargetBreak::BREAK_COVFUN)) {
			g_target->breakDelete(token->m_addr, TargetBreak::BREAK_COVFUN);
		}
	}

	return 0;
}

int DwfCu::funCovReset(DwfFun *fun)
{
	UTER_ASSERT(fun != NULL);

	QMap<unsigned long long, CovToken*>::iterator iter_token;
	for (iter_token = fun->m_cov_token.m_utertoken_map.begin();
		 iter_token != fun->m_cov_token.m_utertoken_map.end(); iter_token++) {
		CovToken *token =  iter_token.value();
		token->m_hit_for_fun = false;
	}

	return 0;
}

int DwfCu::funCovCount(DwfFun *fun, unsigned int &hit, unsigned int &num)
{
	UTER_ASSERT(fun != NULL);
	hit = 0;
	num = 0;

	FunCovInfo *covinfo = funCovInfoGet(fun);

	QMap<FileLineNo, FunCovLine*>::iterator iterline;
	for (iterline = covinfo->m_cov_line_map.begin(); iterline != covinfo->m_cov_line_map.end(); iterline++) {
		FunCovLine *covline = iterline.value();
		QList<FunCovStat*>::iterator iterstat;
		for (iterstat = covline->m_stat_list.begin(); iterstat != covline->m_stat_list.end(); iterstat++) {
			FunCovStat *covstat = *iterstat;
			num++;
			if (covstat->m_count_fun != 0) {
				hit++;
			}
		}
	}

	delete covinfo;
	return 0;
}

/*生成函数覆盖描述信息，到out字符串里*/
int DwfCu::funCovShow(DwfFun *fun, QString &out)
{
	UTER_ASSERT(fun != NULL);
	out = "";

	/*获取函数的覆盖信息，注意后边是要释放的*/
	FunCovInfo *covinfo = funCovInfoGet(fun);

	/*先于一个函数的代码只能在一个文件中，如果分布再多个文件中，就不给打印了。*/
	unsigned int last_file_no = 0xffffffff;
	/*在一个文件内行号的上下界*/
	unsigned int lineno_begin;
	unsigned int lineno_end;
	QMap<FileLineNo, FunCovLine*>::iterator iter;
	for (iter = covinfo->m_cov_line_map.begin(); iter != covinfo->m_cov_line_map.end(); iter++) {
		FileLineNo fileline = iter.key();
		if (last_file_no == 0xffffffff) {
			last_file_no = fileline.m_file_no;

			lineno_begin = fileline.m_line_no;
			lineno_end = fileline.m_line_no;
		} else {
			if (last_file_no != fileline.m_file_no) {
				out += "Source of function in more file.";
				delete covinfo;
				return 0;
			}

			if (fileline.m_line_no < lineno_begin) {
				lineno_begin = fileline.m_line_no;
			}
			if (fileline.m_line_no > lineno_end) {
				lineno_end = fileline.m_line_no;
			}
		}
	}
	if (last_file_no == 0xffffffff) {
		out += "No line information.";
		delete covinfo;
		return 0;
	}

	/*获得源文件路径，和打开源文件*/
	QMap<unsigned int, QString>::iterator iter_filename;
	iter_filename = covinfo->m_filename_map.find(last_file_no);
	if (iter_filename == covinfo->m_filename_map.end()) {
		out += "Source of function with invalid path.";
		delete covinfo;
		return 0;
	}
	QString source_path = iter_filename.value();
	FILE *src_fd = fopen(source_path.toStdString().c_str(), "r");
	if (src_fd == NULL) {
		out += "Open source file failed.";
		delete covinfo;
		return 0;
	}

	/*一边打印数据，一遍统计信息(统计信息最后要打印的)*/
	unsigned int sum = 0;
	unsigned int covs = 0;

	/*行号从1开始*/
	unsigned int iloop = 1;
	while(1) {
		/*一行的最大字符数，再多就不管了*/
		const int buf_size = 2048;
		char read_buf[buf_size];

		/*读取一行*/
		char *line_text = fgets(read_buf, buf_size, src_fd);
		if (line_text == NULL) {
			break;
		}

		/*查找覆盖信息*/
		iter = covinfo->m_cov_line_map.find(FileLineNo(last_file_no, iloop));
		if (iter == covinfo->m_cov_line_map.end()) {
			/*没有覆盖信息的行，如果是在一定的区间的话，比如被打印其余以上和以下的部分，也是要打印的*/
			const unsigned int bound_size = 5;
			/*开始位置比边界值还小时，放弃上边界检查。否则位置要上界往下*/
			if ((lineno_begin < bound_size || iloop >= lineno_begin - bound_size)
				&& (iloop <= lineno_end + bound_size) /*下边界往上*/) {
				out += QString::asprintf("%03d:-/-:%s", iloop, line_text);
			}
		} else {
			/*查找到这一行的覆盖信息，一定要打印*/
			FunCovLine *covline = iter.value();
			/*统计这行的汇编覆盖*/
			unsigned int left = 0;
			unsigned int rigth = 0;
			QList<FunCovStat*>::iterator iter;
			for (iter = covline->m_stat_list.begin(); iter != covline->m_stat_list.end(); iter++) {
				FunCovStat *cstat = *iter;
				left += cstat->m_count_fun;
				rigth += 1;
			}
			/*输出带覆盖前缀的行*/
			out += QString::asprintf("%03d:%d/%d:%s", iloop, left, rigth, line_text);
			/*总统计*/
			covs += left;
			sum += rigth;
		}

		iloop++;
	}

	fclose(src_fd);
	delete covinfo;

	/*覆盖信息总结*/
	out += "======================Coverage information=============================\n";
	out += "Source file:" + m_path + "\n";
	if (sum != 0) {
		out += QString::asprintf("Coverage %u/%u   %u%s \n", covs, sum, covs * 100 / sum, "%");
	}

	return 0;
}
