/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-09-27
Description: 设备隐患排查记录审核
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm202av_f4)

int f_tmsm202av_f4(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_inq = " ";
	CString msgstr = "提示信息:";	//提示信息
	CString s_userid("");
	CString	datetime("");
	int perd_dd = 0;
	CDateTime time_1;
	CDateTime time_2;
	CDateTime accstarttime;
	CDateTime accendtime;
	CModel ttmsm202("TTMSM202");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	int proc_sum = 0;				//操作总数

	try
	{


		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Rows[i]["CHECK_FLAG"].ToString().Trim() == "1")
			{
				sprintf(s.msg, "该条设备记录已整改，请选择未整改的记录！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm202.Reset();
			ttmsm202.MergeFrom(bcls_rec->Tables[0].Rows[i]);





			//ttmsm202["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
			Log::Trace("", __FUNCTION__, bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());



			//查询是否有该日期和序号的记录，更新CHECK_FLAG由0变为1
			sqlstr = "SELECT count(*) FROM TTMSM202 WHERE DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO ";
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm202["DATE_C"].ToString());
			cmd1.Parameters.Set("SEQ_NO", ttmsm202["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "seq_no= {0}", ttmsm202["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "date_ccc= [{0}][{1}]", ttmsm202["DATE_C"].ToString(), ttmsm202["SEQ_NO"].ToString());

			//CDecimal ss = cmd1.ExecuteScalar();
			//Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", ss);
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();
			Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", con);
			if (con>0)
			{
				Log::Trace("", __FUNCTION__, "这里1", "");

				ttmsm202["FINISH_TIME"] = datetime.SubstringNE(0, 8);		//记录实际更换时间
				ttmsm202["CHECK_FLAG"] = 1; //设置是否更换标识
				ttmsm202["REC_ERASOR"] = s.userid;			//记录更换责任者
				ttmsm202["REC_REVISE_TIME"] = datetime;		//记录更换时间


				Log::Trace("", __FUNCTION__, "这里2", "");
				ttmsm202.Update("CHECK_FLAG,FINISH_TIME", "DATE_C,SEQ_NO");
				Log::Trace("", __FUNCTION__, "这里3", "");

				


			}

		}





	}


	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


