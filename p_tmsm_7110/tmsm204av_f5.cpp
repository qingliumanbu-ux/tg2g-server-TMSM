/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-10-15
Description: 备用设备测试记录主管取消审核
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm204av_f5)

int f_tmsm204av_f5(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CModel ttmsm204("TTMSM204");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	CDbCommand cmd_inq(conn);
	int proc_sum = 0;				//操作总数
	EIClass tmp;
	try
	{
		
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Rows[i]["CHECK_FLAG"].ToString().Trim() == "0")
			{
				sprintf(s.msg, "该条记录未测试，请选择已测试的记录！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			
			ttmsm204.Reset();
			ttmsm204.MergeFrom(bcls_rec->Tables[0].Rows[i]);


			//ttmsm204["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
			Log::Trace("", __FUNCTION__, bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());
			

			//查询是否有该日期和序号的记录，更新CHECK_FLAG由1变为0
			sqlstr = "SELECT count(*) FROM TTMSM204 WHERE DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO ";
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm204["DATE_C"].ToString());
			cmd1.Parameters.Set("SEQ_NO", ttmsm204["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "seq_no= {0}", ttmsm204["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "date_ccc= [{0}][{1}]", ttmsm204["DATE_C"].ToString(), ttmsm204["SEQ_NO"].ToString());

			//CDecimal ss = cmd1.ExecuteScalar();
			//Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", ss);
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();
			Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", con);
			if (con > 0)
			{
				Log::Trace("", __FUNCTION__, "这里1", "");
				//查询已测试设备记录的最大序号，如果选择的记录是最大序号，则允许取消，否则不允许取消
				sqlstr_inq = "SELECT MAX(SEQ_NO) MAX_SEQ_NO FROM TTMSM204 WHERE WORK_AREA= @WORK_AREA AND LINE_DESC=@LINE_DESC AND DEV_NAME=@DEV_NAME AND CHECK_FLAG=1";
			
				cmd_inq.SetCommandText(sqlstr_inq);
				cmd_inq.Parameters.Set("WORK_AREA", ttmsm204["WORK_AREA"].ToString());
				cmd_inq.Parameters.Set("LINE_DESC", ttmsm204["LINE_DESC"].ToString());
				cmd_inq.Parameters.Set("DEV_NAME", ttmsm204["DEV_NAME"].ToString());
				cmd_inq.ExecuteQuery(tmp.Tables[0]);
				cmd_inq.Close();
				Log::Trace("", __FUNCTION__, "max_seq_no= {0}", tmp.Tables[0].Rows[0]["MAX_SEQ_NO"].ToString().Trim());
				Log::Trace("", __FUNCTION__, "seq_no= {0}", ttmsm204["SEQ_NO"].ToDecimal().ToInt32());
				if (ttmsm204["SEQ_NO"].ToDecimal().ToInt32() == tmp.Tables[0].Rows[0]["MAX_SEQ_NO"].ToDecimal().ToInt32())
				{
					ttmsm204["TIME_3"] = " ";//实际测试时间设空值
					ttmsm204["CHECK_FLAG"] = 0; //设置审核标识
					ttmsm204["REC_ERASOR"] = s.userid;			//记录审核责任者
					ttmsm204["REC_REVISE_TIME"] = datetime;		//记录审核时间

					Log::Trace("", __FUNCTION__, "这里2", "");
					ttmsm204.Update("CHECK_FLAG,TIME_3", "DATE_C,SEQ_NO");
					Log::Trace("", __FUNCTION__, "这里3", "");
					//删除系统自动添加的那行记录
					sqlstr = "DELETE FROM TTMSM204 WHERE WORK_AREA=@WORK_AREA AND LINE_DESC=@LINE_DESC AND DEV_NAME=@DEV_NAME AND SEQ_NO>@SEQ_NO AND DATE_C='" + datetime.SubstringNE(0, 8) + "'";
					Log::Trace("", __FUNCTION__, "sqlcount[{0}]", sqlstr);
					cmd.SetCommandText(sqlstr);
					cmd.Parameters.Set("WORK_AREA", ttmsm204["WORK_AREA"]);
					cmd.Parameters.Set("LINE_DESC", ttmsm204["LINE_DESC"]);
					cmd.Parameters.Set("DEV_NAME", ttmsm204["DEV_NAME"]);
					cmd.Parameters.Set("SEQ_NO", ttmsm204["SEQ_NO"]);
					cmd.ExecuteNonQuery();

				
				}
				else
				{
					sprintf(s.msg, "只允许取消最近一次的已测试记录，之前的已测试记录不能取消！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				

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


