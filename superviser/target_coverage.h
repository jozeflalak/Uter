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

#ifndef SRC_TARGET_COVERAGE_H_
#define SRC_TARGET_COVERAGE_H_

/*File number and line number, for compare in map*/
class FileLineNo {
public:
	FileLineNo()
	{
		m_file_no = 0;
		m_line_no = 0;
		return;
	}
	FileLineNo(unsigned int file_no, unsigned int line_no)
	{
		m_file_no = file_no;
		m_line_no = line_no;
		return;
	}
    bool operator==(const FileLineNo& cmp) const
    {
        return (m_file_no == cmp.m_file_no) && (m_line_no == cmp.m_line_no);
    }
    bool operator<(const FileLineNo& cmp) const
    {
        if (m_file_no < cmp.m_file_no) {
        	return true;
        } else if (m_file_no > cmp.m_file_no) {
        	return false;
        } else {
        	if (m_line_no < cmp.m_line_no) {
        		return true;
        	} else {
        		return false;
        	}
        }
    }
	unsigned int m_file_no;
	unsigned int m_line_no;
};

/*Statement for coverage*/
class FunCovStat {
public:
	FunCovStat()
	{
		m_addr = 0;
		m_count_fun = false;
		return;
	}
	unsigned long long m_addr;			/*Begin address of statement*/
	bool m_count_fun;					/*Coverage count of statement*/
};

/*Source line for coverage, include some FunCovStat*/
class FunCovLine {
public:
	~FunCovLine()
	{
		QList<FunCovStat*>::iterator iter;
		for (iter = m_stat_list.begin(); iter != m_stat_list.end(); /*none*/) {
			FunCovStat *cov_stat = *iter;
			delete cov_stat;
			m_stat_list.erase(iter++);
		}
		return;
	}
	FileLineNo m_fileline_no;					/*File line number*/
	QList<FunCovStat*> m_stat_list;				/*Statements in file line*/
};

/*Function coverage result for export*/
class FunCovInfo {
public:
	~FunCovInfo()
	{
		QMap<FileLineNo, FunCovLine*>::iterator iter;
		for (iter = m_cov_line_map.begin(); iter != m_cov_line_map.end(); /*none*/) {
			FunCovLine *covline = iter.value();
			delete covline;
			m_cov_line_map.erase(iter++);
		}
		return;
	}
	QMap<unsigned int, QString> m_filename_map;			/*Source file number to file path*/
	QMap<FileLineNo, FunCovLine*> m_cov_line_map;		/*Source file coverage which in then function*/
};

/*Coverage token, make some token in function with jump from and jump to*/
class CovToken {
public:
	CovToken()
	{
		m_addr = 0;
		m_hit_for_fun = false;
		m_hit_for_file = false;
		return;
	}
	unsigned long long m_addr;			/*Token address at, the last instruction in the token area*/
	bool m_hit_for_fun;					/*Hit count for function, use it in python*/
	bool m_hit_for_file;				/*Hit count for file, uise it in UI*/
};

/*Tokens in function*/
class CovTokenSet {
public:
	~CovTokenSet()
	{
		QMap<unsigned long long, CovToken*>::iterator token_iter;
		for (token_iter = m_utertoken_map.begin(); token_iter != m_utertoken_map.end(); /*none*/) {
			CovToken *token = token_iter.value();
			delete token;
			m_utertoken_map.erase(token_iter++);
		}
		m_addtoken_map.clear();
		return;
	}
	CovToken *getTokenByAddr(unsigned long long addr)
	{
		QMap<unsigned long long, CovToken*>::iterator token_iter;
		token_iter = m_addtoken_map.find(addr);
		if (token_iter == m_addtoken_map.end()) {
			return NULL;
		}
		return token_iter.value();
	}
	int loadFromAsmSet(BfdAsmSet *asmset);					/*Make token set from disassembler*/
	QMap<unsigned long long, CovToken*> m_utertoken_map;	/*All instructions to token which token at, release token by this map*/
	QMap<unsigned long long, CovToken*> m_addtoken_map;		/*All instructions to token*/

	static int setTokenSet(TargetMod *mod);
};

#endif /*SRC_TARGET_COVERAGE_H_*/
