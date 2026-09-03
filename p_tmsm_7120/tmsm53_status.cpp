/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:行车作业命令修改
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件


//外部函数声明
int f_tmsm53_status_chg(EIClass* bcls_rec, EIClass* bcls_ret);

BM2F_ENTERACE(tmsm53_status)

int f_tmsm53_status(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int ret = 0;
	CString crane_inst_status("");//行车状态
	CString crane_inst_no("");//行车命令号

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm53("TTMSM53");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{

			EIClass inBlock;
			EIClass outBlock;

			if (bcls_rec->Tables[0].Columns.Contains("CRANE_INST_NO"))
			{
				crane_inst_no = bcls_rec->Tables[0].Rows[0]["CRANE_INST_NO"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("CRANE_INST_STATUS"))
			{
				crane_inst_status = bcls_rec->Tables[0].Rows[0]["CRANE_INST_STATUS"].ToString();
			}
			Log::Info("", __FUNCTION__, "crane_inst_no=[{0}]", crane_inst_no);
			Log::Info("", __FUNCTION__, "crane_inst_status=[{0}]", crane_inst_status);
			inBlock.Tables.Clear();
			inBlock.Tables.Add();
			inBlock.Tables[0].Columns.Add(DT_STRING,"CRANE_INST_NO");
			inBlock.Tables[0].Columns.Add(DT_STRING, "CRANE_INST_STATUS");
			inBlock.Tables[0].Rows.Clear();
			inBlock.Tables[0].Rows.Add();
			inBlock.Tables[0].Rows[0]["CRANE_INST_NO"]= crane_inst_no;
			inBlock.Tables[0].Rows[0]["CRANE_INST_STATUS"]= crane_inst_status;
			ret = f_tmsm53_status_chg(&inBlock,&outBlock);
			if (ret != 0)
			{
				strcpy(s.msg, "更新状态失败！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}

