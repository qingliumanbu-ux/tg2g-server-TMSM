/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2012
Author:		hk
Version:    1.0
Date:		2014-11-11
Description:操作履历函数
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件




extern "C"
int f_tmsm97_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	//程序用变量
	int i =0;
	int doFlag = 0;
	int fetchRowCount;
	CString	date_time = "";
	int v_mat_seq_no = 0;
	int	v_mat_seq_no_new = 0;
	int blkNum = 0;

	// 数据库SQL操作字符串
	CString sqlstr("");

	// 实体类定义
	CModel ttmsm97("TTMSM97");

	// 数据库操作类定义
	CDbCommand cmd_inq(conn);
	

	CTracer log(__FUNCTION__);
	try
	{

		//获得输入参数
		date_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
		blkNum = bcls_rec->Tables.IndexOf("TM0099");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块TM0099不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for (int i = 0; i < bcls_rec->Tables["TM0099"].Rows.get_Count(); i++)
		{
			sqlstr = " select to_char(CURRENT_TIMESTAMP,'YYYYMMDDHH24MISSFF6')  from dual";
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", (const char*)sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				ttmsm97["RESUME_SEQ_NO"] = cmd_inq.GetString(1); /*将数据获取到实体对象中*/
			}
			cmd_inq.Close();
			Log::Trace("", __FUNCTION__, "ttmsm97.RESUME_SEQ_NO =[{0}]", (const char*)ttmsm97["RESUME_SEQ_NO"].ToString());
			ttmsm97["REC_CREATE_TIME"] = date_time;
			ttmsm97["REC_CREATOR"] = s.userid;
			ttmsm97["FORM_NAME"] = s.formname;
			ttmsm97["FUNC_ID"] = s.svc_name;
			ttmsm97.Insert();
		}
		

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
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

	return doFlag;

}
