/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    KE2111
Version:    1.0
Date:     2024-02-16
Description:
**************************************************/
//框架公用头文件，勿删
#include "stdafx.h"

/******后台pc文件标准注释标记*****/

/* ***** 静态函数申明 ***** */


/*<remark>=========================================================

===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm13av_ins)

int f_tmsm13av_ins(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义	

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int doflag = 0;
	int fetchRowCount = 0;
	int i;
	int ret;
	int RowCount = 0;
	int blkNum = 0;
	CDecimal i_count = 0;
	CDecimal i_status = 0;
	CString v_shift_no("");
	CString v_shift_group("");
	CString sqlstr("");
	CString sqlstr1("");
	CString code_class("");
	CString time("");
	CString ladle_type("");

	CModel ttmsm13("TTMSM13");


	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	EIClass tmp;
	EIClass tmp1;


	try
	{
		if (bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString() == "A")
		{
			code_class = "TM01";
			ladle_type = "TS";
		}
		if (bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString() == "B")
		{
			code_class = "TM02";
			ladle_type = "TC";
		}

		if (bcls_rec->Tables[0].Rows[0]["SHIFT_NO"].ToString() == "1")
		{
			time = "000001";
		}
		if (bcls_rec->Tables[0].Rows[0]["SHIFT_NO"].ToString() == "2")
		{
			time = "080001";
		}
		if (bcls_rec->Tables[0].Rows[0]["SHIFT_NO"].ToString() == "3")
		{
			time = "160001";
		}
		f_epep_get_shift_group("SM", bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().Substring(0, 8) + time, v_shift_no, v_shift_group, conn);
		ttmsm13["REC_CREATOR"] = s.userid;
		ttmsm13["REC_CREATE_TIME"] = datetimeNow;
		ttmsm13["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().Substring(0,8);
		ttmsm13["SHIFT_NO"] = bcls_rec->Tables[0].Rows[0]["SHIFT_NO"].ToString();
		ttmsm13["SHIFT_GROUP"] = v_shift_group;
		ttmsm13["KEY_1"] = ttmsm13["DATE_C"].ToString() + ttmsm13["SHIFT_NO"].ToString();
		ttmsm13["C_DIV"] = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString();

		if (ttmsm13.QueryCount("DATE_C,SHIFT_NO") > 0)
		{
			ttmsm13.Delete("DATE_C,SHIFT_NO");
		}

		EIClass tmp;
		//sqlstr updated by gnn on 20240511 
		sqlstr = " select * from ttmsm13 where KEY_1=(select max(KEY_1) from ttmsm13 where KEY_1<'" + ttmsm13["KEY_1"].ToString() + "' and C_DIV ='" + ttmsm13["C_DIV"].ToString() + "') ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}{1}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(tmp.Tables[0]);
		cmd_inq.Close();


		sqlstr = " select CODE from TWMSMZD02 where CODE_CLASS='" + code_class + "' ";
		Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			ttmsm13["LADLE_NO"] = cmd_inq.GetString(1);
			sqlstr1 = " select LADLE_LIFE, NOZZLE_BRICK_LIFE, UP_NOZZLE_LIFE, SLIDE_LIFE, DOWN_NOZZLE_LIFE, BOTTOM_BLOW1_LIFE, BOTTOM_BLOW2_LIFE from ttmsm12 WHERE LADLE_NO='" 
				+ ttmsm13["LADLE_NO"].ToString() + "' AND REC_CREATE_TIME=(SELECT MAX(REC_CREATE_TIME) FROM TTMSM12 "+
				" WHERE LADLE_NO='" + ttmsm13["LADLE_NO"].ToString() + "' AND REPAIR_SHIFT_NO='" + bcls_rec->Tables[0].Rows[0]["SHIFT_NO"].ToString() + "') ";
			cmd_inq1.SetCommandText(sqlstr1);
			cmd_inq1.ExecuteQuery(tmp1.Tables[0]);
			cmd_inq1.Close();
			Log::Trace("", __FUNCTION__, "sqlstr1		= [{0}]", sqlstr1);
			ttmsm13["LADLE_NO"] = ladle_type + ttmsm13["LADLE_NO"].ToString();
			for (int i = 0; i < tmp.Tables[0].Rows.get_Count(); i++)
			{
				if (tmp.Tables[0].Rows[i]["LADLE_NO"].ToString() == ttmsm13["LADLE_NO"].ToString())
				{
					ttmsm13.MergeFrom(tmp.Tables[0].Rows[i]);
					
					ttmsm13["REC_CREATOR"] = s.userid;
					ttmsm13["REC_CREATE_TIME"] = datetimeNow;
					ttmsm13["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().Substring(0, 8);
					ttmsm13["SHIFT_NO"] = bcls_rec->Tables[0].Rows[0]["SHIFT_NO"].ToString();
					ttmsm13["SHIFT_GROUP"] = v_shift_group;
					ttmsm13["KEY_1"] = ttmsm13["DATE_C"].ToString() + ttmsm13["SHIFT_NO"].ToString();
					ttmsm13["C_DIV"] = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString();
					if (tmp1.Tables[0].Rows.get_Count() > 0)
					{

						ttmsm13["LADLE_LIFE"] = tmp1.Tables[0].Rows[0]["LADLE_LIFE"].ToString();
						ttmsm13["NOZZLE_BRICK_LIFE"] = tmp1.Tables[0].Rows[0]["NOZZLE_BRICK_LIFE"].ToString();
						ttmsm13["UP_NOZZLE_LIFE"] = tmp1.Tables[0].Rows[0]["UP_NOZZLE_LIFE"].ToString();
						ttmsm13["SLIDE_LIFE"] = tmp1.Tables[0].Rows[0]["SLIDE_LIFE"].ToString();
						ttmsm13["DOWN_NOZZLE_LIFE"] = tmp1.Tables[0].Rows[0]["DOWN_NOZZLE_LIFE"].ToString();
						ttmsm13["BOTTOM_BLOW1_LIFE"] = tmp1.Tables[0].Rows[0]["BOTTOM_BLOW1_LIFE"].ToString();
						ttmsm13["BOTTOM_BLOW2_LIFE"] = tmp1.Tables[0].Rows[0]["BOTTOM_BLOW2_LIFE"].ToString();
					}
				}
			}
			ttmsm13.Insert();
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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



	return doFlag;

}
