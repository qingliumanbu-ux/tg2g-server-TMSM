/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-03-02
Description: 连铸机设备周期管理扇形段记录导入
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm171av_f3)

int f_tmsm171av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString s_userid("");
	CString	datetime("");
	int seq_no = 0;
	int cseq_no = 0;
	int nseq_no = 0;
	int n = 0;
	CDateTime endtime;
	CDateTime onlinetime;
	CDateTime curtime;
	int mon_num = 0;
	CModel ttmsm171("TTMSM171");
	CModel ttmsm171C("TTMSM171");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	try
	{

		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm171.Reset();
			ttmsm171.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			ttmsm171C.Reset();
			ttmsm171C.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			
			//Log::Trace("", __FUNCTION__, "QUERY_DATE_C= {0}", bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString().SubstringNE(0, 10).Remove("-"));


			curtime = CDateTime::Parse(datetime.SubstringNE(0,8));



			if (ttmsm171["ON_LINE_TIME"].ToString().Trim() != "")
			{
				onlinetime = CDateTime::Parse(bcls_rec->Tables[0].Rows[i]["ON_LINE_TIME"].ToString().SubstringNE(0, 8));//上线时间
				//endtime = CDateTime::Parse(bcls_rec->Tables[0].Rows[i]["END_TIME"].ToString().SubstringNE(0, 8));//到期时间
				mon_num = bcls_rec->Tables[0].Rows[i]["MON_NUM"].ToDecimal().ToInt32();
				endtime = onlinetime.AddMonths(mon_num);
				Log::Trace("", __FUNCTION__, "ENDTIME= {0}", endtime.ToString("yyyyMMdd"));
				ttmsm171["END_TIME"] = endtime.ToString("yyyyMMddHHmmss");
				CTimeSpan timespan = endtime.Subtract(curtime);
				CDecimal totalDays = timespan.TotalDays();
				//onlinetime = CDateTime::Parse("20240420");//上线时间
				//endtime = CDateTime::Parse("20240426");//到期时间
				//ttmsm171["DAYS"] = CString::Format("{0}", totalDays.Round(0));
				if (totalDays.Round(0) >= 0)
				{
					ttmsm171["DAYS"] = totalDays.Round(0);
				}
				else
				{
					ttmsm171["DAYS"] = 0;
				}

			
				//Log::Trace("", __FUNCTION__, "20240426 - 20240420 = {0}", totalDays.Round(0));
			}
			else
			{
				ttmsm171["DAYS"] = NULL;
			}
			/*
			if (ttmsm171["END_TIME"].ToString().Trim() != "")
			{

				endtime = CDateTime::Parse(bcls_rec->Tables[0].Rows[i]["END_TIME"].ToString().SubstringNE(0, 8));//到期时间
				CTimeSpan timespan = endtime.Subtract(curtime);
				CDecimal totalDays = timespan.TotalDays();
				//onlinetime = CDateTime::Parse("20240420");//上线时间
				//endtime = CDateTime::Parse("20240426");//到期时间
				//ttmsm171["DAYS"] = CString::Format("{0}", totalDays.Round(0));
				if (totalDays.Round(0) >= 0)
				{
					ttmsm171["DAYS"] = totalDays.Round(0);
				}
				else
				{
					ttmsm171["DAYS"] = 0;
				}


				Log::Trace("", __FUNCTION__, "20240426 - 20240420 = {0}", totalDays.Round(0));
			}
			else
			{
				ttmsm171["DAYS"] = NULL;
			}
			*/
			//查询是否有该日期和序号的记录，有记录更新，没有记录新增
		
			sqlstr = "SELECT count(*) FROM TTMSM171 WHERE DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO ";
		
			
			
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm171["DATE_C"].ToString());
			cmd1.Parameters.Set("SEQ_NO", ttmsm171["SEQ_NO"].ToString());

			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();

			if (con>0)
			{
				ttmsm171["REC_REVISOR"] = s.userid;			//记录修改责任者
				ttmsm171["REC_REVISE_TIME"] = datetime;		//记录修改时刻
				ttmsm171.Update("CCMACH_DESC,LOCATION,SD_NO,ON_LINE_TIME,MON_NUM,END_TIME,DAYS,REC_REVISOR,REC_REVISE_TIME", "DATE_C,SEQ_NO");

			}
			else
			{
				if (bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString().Trim() == ""  && bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().Trim() == "")
				{
					ttmsm171["DATE_C"] = datetime.SubstringNE(0, 8);//填表日期为当前日期

				}
				else
				{
					ttmsm171["DATE_C"] = bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString().SubstringNE(0, 10).Remove("-");//填表日期为查询日期

				}
				
				ttmsm171["SEQ_NO"] = bcls_rec->Tables[0].Rows[i]["SEQ_NO"].ToString();//序号
				ttmsm171["REC_CREATOR"] = s.userid;			//记录创建责任者
				ttmsm171["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				ttmsm171.Insert();

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


