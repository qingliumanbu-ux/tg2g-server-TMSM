/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:行车状态信息新增
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件
  

//外部函数声明
int EPGetNextSeq(const char* seq_name, char* tcseq);

BM2F_ENTERACE(tmsm52_ins)                                         

int f_tmsm52_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	


	/* 实体类定义 */ 
	CModel ttmsm52("TTMSM52");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			ttmsm52.Reset();
			ttmsm52.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm52.TrimOrBlank();
			

			ttmsm52["REC_CREATOR"]			= s.userid;		//记录创建责任者
			ttmsm52["REC_CREATE_TIME"]		= datetime;		//记录创建时刻
			ttmsm52["IP_ADDR"] = s.fore_ip;		//记录IP_ADDR
			ttmsm52["REPAIR_SEQ_NO"] = EPGetNextSeq("TTMSM57_SEQ_NO", conn);
			ttmsm52.Print();
			/* 查询该钢包号是否存在 */
			if (ttmsm52.QueryCount("CRANENO") > 0)
			{
				sprintf(s.msg, "行车号[%s]已存在", (const char*)ttmsm52["CRANENO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm52.Insert();
			
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

