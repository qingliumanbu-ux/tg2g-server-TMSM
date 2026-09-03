/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-15 15:37:03
Description: 钢包下渣检测修改
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme34a1_upd)


int f_tmsme34a1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm34("TTMSM34");

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
			ttmsm34.Reset();
			ttmsm34.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm34.TrimOrBlank();

			/* 检查输入参数合法性 */
			if (ttmsm34["HEAT_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "熔炼号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 修改包盖基本信息表 */
			ttmsm34["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm34["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm34["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm34["REC_REVISE_TIME"] = datetime;		//记录修改时刻

			sqlstr = "UPDATE TTMSM34 SET REC_REVISOR= '" + ttmsm34["REC_REVISOR"].ToString() + "',"
				"REC_REVISE_TIME = '" + ttmsm34["REC_REVISE_TIME"].ToString() + "',"
				"LADLE_NO = '" + ttmsm34["LADLE_NO"].ToString() + "',"
				"ST_NO = '" + ttmsm34["ST_NO"].ToString() + "',"
				"CC_MACH_NO = '" + ttmsm34["CC_MACH_NO"].ToString() + "',"
				"CAST_ARM_NO = " + ttmsm34["CAST_ARM_NO"].ToString() + ","
				"LADLE_LIFE = '" + ttmsm34["LADLE_LIFE"].ToString() + "',"
				"USAGE_ST = '" + ttmsm34["USAGE_ST"].ToString() + "',"
				"COIL_NAME = '" + ttmsm34["COIL_NAME"].ToString() + "',"
				"COIL_USETIMES = " + ttmsm34["COIL_USETIMES"].ToString() + ","
				"COIL_CHANGE_TIME = '" + ttmsm34["COIL_CHANGE_TIME"].ToString() + "',"
				"SOCKET_NAME = '" + ttmsm34["SOCKET_NAME"].ToString() + "',"
				"SOCKET_CHANGE_TIME = '" + ttmsm34["SOCKET_CHANGE_TIME"].ToString() + "',"
				"SOCKET_USETIMES = " + ttmsm34["SOCKET_USETIMES"].ToString() + ","
				"USAGE1 = " + ttmsm34["USAGE1"].ToString() + " "
			" WHERE HEAT_NO = " + ttmsm34["HEAT_NO"].ToString() + "";
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


