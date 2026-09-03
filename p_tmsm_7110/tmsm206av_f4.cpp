/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2025-02-17
Description: 大功率电机测试记录主管审核
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm206av_f4)

int f_tmsm206av_f4(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CModel ttmsm206("TTMSM206");
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
				sprintf(s.msg, "该条记录已测试，请选择未测试的记录！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm206.Reset();
			ttmsm206.MergeFrom(bcls_rec->Tables[0].Rows[i]);





			//ttmsm206["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
			Log::Trace("", __FUNCTION__, bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());



			//查询是否有该日期和序号的记录，更新CHECK_FLAGWEI由0变为1
			sqlstr = "SELECT count(*) FROM TTMSM206 WHERE DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO ";
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm206["DATE_C"].ToString());
			cmd1.Parameters.Set("SEQ_NO", ttmsm206["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "seq_no= {0}", ttmsm206["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "date_ccc= [{0}][{1}]", ttmsm206["DATE_C"].ToString(), ttmsm206["SEQ_NO"].ToString());

			//CDecimal ss = cmd1.ExecuteScalar();
			//Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", ss);
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();
			Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", con);
			if (con > 0)
			{
				Log::Trace("", __FUNCTION__, "这里1", "");

				ttmsm206["TIME_3"] = datetime.SubstringNE(0, 8);		//记录实际测试时间
				ttmsm206["CHECK_FLAG"] = 1; //设置审核标识
				ttmsm206["REC_ERASOR"] = s.userid;			//记录审核责任者
				ttmsm206["REC_REVISE_TIME"] = datetime;		//记录审核时间

				Log::Trace("", __FUNCTION__, "这里2", "");
				ttmsm206.Update("CHECK_FLAG,TIME_3", "DATE_C,SEQ_NO");
				Log::Trace("", __FUNCTION__, "这里3", "");

				//当更换标识为1时，自动新增一条记录，上次维护时间默认系统时间，下次计划时间按周期计算。

				ttmsm206["DATE_C"] = datetime.SubstringNE(0, 8);//填表日期为当前日期
				ttmsm206["CHECK_FLAG"] = 0; //设置是否更换标识
				time_1 = CDateTime::Parse(datetime.SubstringNE(0, 8));//上次维护时间
				ttmsm206["TIME_1"] = time_1.ToString("yyyyMMdd");
				perd_dd = bcls_rec->Tables[0].Rows[i]["PERD_DD"].ToDecimal().ToInt32();
				time_2 = time_1.AddDays(perd_dd); //下次计划时间=上次维护时间+周期天数
				Log::Trace("", __FUNCTION__, "TIME_2= {0}", time_2.ToString("yyyyMMdd"));
				ttmsm206["TIME_2"] = time_2.ToString("yyyyMMdd");
				ttmsm206["TIME_3"] = " ";
				ttmsm206["REC_CREATOR"] = s.userid;			//记录创建责任者
				ttmsm206["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				//默认序号为1，自动累加
				CDecimal st_seq_no = 1;
				sqlstr_inq = " SELECT MAX(SEQ_NO) FROM TTMSM206";
				Log::Trace("", __FUNCTION__, "str_sql_inq		= [{0}]", sqlstr_inq);
				cmd.SetCommandText(sqlstr_inq);
				cmd.ExecuteReader();
				if (cmd.Read())
				{
					st_seq_no = cmd.GetDecimal(1) + 1;
				}
				ttmsm206["SEQ_NO"] = st_seq_no;		//序号

				ttmsm206.Insert();//插入一条新记录
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


