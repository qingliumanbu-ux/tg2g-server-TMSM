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
BM2F_ENTERACE(tmsm13av_inq)

int f_tmsm13av_inq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
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
	CModel ttmsm13("TTMSM13");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);

	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString datetimeNow_l = CDateTime::Now().AddHours(-8).ToString("yyyyMMddHHmmss");

	EIClass tmp;
	EIClass tmp1;

	try
	{
		CString c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString();
		
		
			sqlstr = " select * from ttmsm13 where 1=1 and DATE_C='" + bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0, 8) + "' and  SHIFT_NO='1' AND C_DIV='" + c_div + "' ";
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " select * from ttmsm13 where 1=1 and DATE_C='" + bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0, 8) + "' and  SHIFT_NO='2' AND C_DIV='" + c_div + "' ";
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
			cmd_inq.Close();

			bcls_ret->Tables.Add();
			sqlstr = " select * from ttmsm13 where 1=1 and DATE_C='" + bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0, 8) + "' and  SHIFT_NO='3' AND C_DIV='" + c_div + "' ";
			Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
			cmd_inq.Close();
		
			if (c_div == "A")
			{
				f_epep_get_shift_group("SMDD", datetimeNow, v_shift_no, v_shift_group, conn);

				bcls_ret->Tables[atoi(v_shift_no) - 1].Rows.Clear();

				ttmsm13["KEY_1"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().Substring(0, 8) + v_shift_no;
				sqlstr = " select * from ttmsm13 where KEY_1=(select max(KEY_1) from ttmsm13 where KEY_1<'" + ttmsm13["KEY_1"].ToString() + "' and C_DIV ='" + c_div + "') ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}{1}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables[atoi(v_shift_no) - 1]);
				cmd_inq.Close();


				sqlstr = " select CODE from TWMSMZD02 where CODE_CLASS='TM01' ";
				Log::Trace("", __FUNCTION__, "sqlstr		= [{0}]", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				while (cmd_inq.Read())
				{
					ttmsm13["LADLE_NO"] = cmd_inq.GetString(1);
					sqlstr1 = " select LADLE_LIFE, NOZZLE_BRICK_LIFE, UP_NOZZLE_LIFE, SLIDE_LIFE, DOWN_NOZZLE_LIFE, BOTTOM_BLOW1_LIFE, BOTTOM_BLOW2_LIFE from ttmsm12 WHERE LADLE_NO='"
						+ ttmsm13["LADLE_NO"].ToString() + "' AND REC_CREATE_TIME=(SELECT MAX(REC_CREATE_TIME) FROM TTMSM12 " +
						" WHERE LADLE_NO='" + ttmsm13["LADLE_NO"].ToString() + "' AND REPAIR_SHIFT_NO='" + v_shift_no + "') ";
					cmd_inq1.SetCommandText(sqlstr1);
					cmd_inq1.ExecuteQuery(tmp1.Tables[0]);
					cmd_inq1.Close();

					Log::Trace("", __FUNCTION__, "sqlstr1		= [{0}]", sqlstr1);
					ttmsm13["LADLE_NO"] = "TS" + ttmsm13["LADLE_NO"].ToString();
					for (int i = 0; i < bcls_ret->Tables[atoi(v_shift_no) - 1].Rows.get_Count(); i++)
					{
						if (bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["LADLE_NO"].ToString() == ttmsm13["LADLE_NO"].ToString())
						{
							CString l_status = Db::QueryCString(" SELECT LADLE_STATUS FROM TTMSM11 WHERE LADLE_NO='" + ttmsm13["LADLE_NO"].ToString() + "' AND RESUME_SEQ_NO =(SELECT MAX(RESUME_SEQ_NO) FROM TTMSM11 WHERE LADLE_NO='" + ttmsm13["LADLE_NO"].ToString() + "') ");
							Log::Trace("", __FUNCTION__, "l_status		= [{0}]", l_status);
							bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["CURRENT_STATUS"] = l_status;
							if (tmp1.Tables[0].Rows.get_Count() > 0)
							{
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["LADLE_LIFE"] = tmp1.Tables[0].Rows[0]["LADLE_LIFE"].ToString();
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["NOZZLE_BRICK_LIFE"] = tmp1.Tables[0].Rows[0]["NOZZLE_BRICK_LIFE"].ToString();
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["UP_NOZZLE_LIFE"] = tmp1.Tables[0].Rows[0]["UP_NOZZLE_LIFE"].ToString();
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["SLIDE_LIFE"] = tmp1.Tables[0].Rows[0]["SLIDE_LIFE"].ToString();
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["DOWN_NOZZLE_LIFE"] = tmp1.Tables[0].Rows[0]["DOWN_NOZZLE_LIFE"].ToString();
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["BOTTOM_BLOW1_LIFE"] = tmp1.Tables[0].Rows[0]["BOTTOM_BLOW1_LIFE"].ToString();
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["BOTTOM_BLOW2_LIFE"] = tmp1.Tables[0].Rows[0]["BOTTOM_BLOW2_LIFE"].ToString();
								bcls_ret->Tables[atoi(v_shift_no) - 1].Rows[i]["SHIFT_GROUP"] = v_shift_group;
							}
							
						}
						
					}
				}
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
