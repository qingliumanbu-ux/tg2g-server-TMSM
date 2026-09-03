/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-24 15:03:57
Description: 扇形段通钢量删除
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme68a1_del)


int f_tmsme68a1_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	//CModel ttmsm66 = CModel("TTMSM66");
	CModel ttmsm68("TTMSM68");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
			Log::Trace("", __FUNCTION__, "datetime		= [{0}]", datetime);
			/* ***** 获取输入参数 ***** */
			if (bcls_rec->Tables[0].Columns.Contains("SEQ_NO"))
				ttmsm68["SEQ_NO"] = bcls_rec->Tables[0].Rows[0]["SEQ_NO"];

			Log::Trace("", __FUNCTION__, "ttmsm68.SEQ_NO		= [{0}]", (const char*)ttmsm68["SEQ_NO"].ToString());

			/* 检查输入参数合法性 */
			if (ttmsm68["SEQ_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "序号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			/* 删除工器具扇形段通钢量信息 */
			ttmsm68.Delete("SEQ_NO");

			//Log::Trace("", "", "sqlstr：{0}", sqlstr);

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


