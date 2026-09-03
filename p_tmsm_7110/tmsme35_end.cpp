/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-14 15:09:48
Description: 包盖维修结束
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme35_end)


int f_tmsme35_end(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm35("TTMSM35");

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
			ttmsm35.Reset();
			ttmsm35.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm35.TrimOrBlank();

			/* 检查输入参数合法性 */
			if (ttmsm35["CLAD_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "包盖号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 修改包盖基本信息表 */
			ttmsm35["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm35["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm35["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm35["REC_REVISE_TIME"] = datetime;		//记录修改时刻

			sqlstr = "UPDATE TTMSM35 SET REC_REVISOR= '" + ttmsm35["REC_REVISOR"].ToString() + "',"
				"REC_REVISE_TIME = '" + ttmsm35["REC_REVISE_TIME"].ToString() + "',"
				"LD_CAP_STATUE = '0',LAST_USAGE_END_TIME = REPAIR_END_TIME ,"   //找个无关的字段存放之前的维修时间
				"REPAIR_END_TIME = '" + ttmsm35["REC_CREATE_TIME"].ToString() + "' "
			" WHERE CLAD_NO = " + ttmsm35["CLAD_NO"].ToString() + "";
			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::Execute(sqlstr);



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


