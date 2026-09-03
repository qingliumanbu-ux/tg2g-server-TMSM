/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-09-27
Description: 设备周期性维护记录审核
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm203av_f4)

int f_tmsm203av_f4(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CModel ttmsm203("TTMSM203");
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
				sprintf(s.msg, "该条设备记录已更换，请选择未更换的记录！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm203.Reset();
			ttmsm203.MergeFrom(bcls_rec->Tables[0].Rows[i]);





			//ttmsm203["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
			Log::Trace("", __FUNCTION__, bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString());



			//查询是否有该日期和序号的记录，更新CHECK_FLAGWEI由0变为1
			sqlstr = "SELECT count(*) FROM TTMSM203 WHERE DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO ";
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm203["DATE_C"].ToString());
			cmd1.Parameters.Set("SEQ_NO", ttmsm203["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "seq_no= {0}", ttmsm203["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "date_ccc= [{0}][{1}]", ttmsm203["DATE_C"].ToString(), ttmsm203["SEQ_NO"].ToString());

			//CDecimal ss = cmd1.ExecuteScalar();
			//Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", ss);
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();
			Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", con);
			if (con>0)
			{
				Log::Trace("", __FUNCTION__, "这里1", "");
				
				ttmsm203["TIME_3"] = bcls_rec->Tables[0].Rows[0]["TIME_3"].ToString().SubstringNE(0, 8);		//记录实际更换时间
				ttmsm203["CHECK_FLAG"] = 1; //设置是否更换标识
				ttmsm203["REC_ERASOR"] = s.userid;			//记录更换责任者
				ttmsm203["REC_REVISE_TIME"] = datetime;		//记录更换时间
			

				Log::Trace("", __FUNCTION__, "这里2", "");
				ttmsm203.Update("CHECK_FLAG,TIME_3", "DATE_C,SEQ_NO");
				Log::Trace("", __FUNCTION__, "这里3", "");

				//当更换标识为1时，自动新增一条记录，上次维护时间默认系统时间，下次计划时间按周期计算。
				ttmsm203["DATE_C"] = datetime.SubstringNE(0, 8);//填表日期为当前日期

				sqlstr = "SELECT count(*) FROM TTMSM203 WHERE WORK_AREA = @WORK_AREA  AND LINE_DESC= @LINE_DESC AND SPACE_DESC= @SPACE_DESC AND DEV_NAME= @DEV_NAME AND ITEM_NAME= @ITEM_NAME and DATE_C = @DATE_C  ";
				cmd1.SetCommandText(sqlstr);
				cmd1.Parameters.Set("WORK_AREA", ttmsm203["WORK_AREA"].ToString());
				cmd1.Parameters.Set("LINE_DESC", ttmsm203["LINE_DESC"].ToString());
				cmd1.Parameters.Set("SPACE_DESC", ttmsm203["SPACE_DESC"].ToString());
				cmd1.Parameters.Set("DEV_NAME", ttmsm203["DEV_NAME"].ToString());
				cmd1.Parameters.Set("ITEM_NAME", ttmsm203["ITEM_NAME"].ToString());
				cmd1.Parameters.Set("DATE_C", ttmsm203["DATE_C"].ToString());
				
				Log::Trace("", __FUNCTION__, "date_ccc= [{0}][{1}]", ttmsm203["DATE_C"].ToString(), ttmsm203["SEQ_NO"].ToString());
				CDecimal con1 = (cmd1.ExecuteScalar()).ToInt32();
				Log::Trace("", __FUNCTION__, "ExecuteScalar= [{0}][{1}]", con1);
				
				if (con1 < 1)
				{
					ttmsm203["CHECK_FLAG"] = 0; //设置是否更换标识
					time_1 = CDateTime::Parse(bcls_rec->Tables[0].Rows[0]["TIME_3"].ToString().SubstringNE(0, 8));//上次维护时间
					ttmsm203["TIME_1"] = time_1.ToString("yyyyMMdd");
					perd_dd = bcls_rec->Tables[0].Rows[i]["PERD_DD"].ToDecimal().ToInt32();
					time_2 = time_1.AddDays(perd_dd); //下次计划时间=上次维护时间+周期天数
					Log::Trace("", __FUNCTION__, "TIME_2= {0}", time_2.ToString("yyyyMMdd"));
					ttmsm203["TIME_2"] = time_2.ToString("yyyyMMdd");
					ttmsm203["TIME_3"] = " ";
					ttmsm203["REC_CREATOR"] = s.userid;			//记录创建责任者
					ttmsm203["REC_CREATE_TIME"] = datetime;		//记录创建时刻
					//默认序号为1，自动累加
					CDecimal st_seq_no = 1;
					sqlstr_inq = " SELECT MAX(SEQ_NO) FROM TTMSM203";
					Log::Trace("", __FUNCTION__, "str_sql_inq		= [{0}]", sqlstr_inq);
					cmd.SetCommandText(sqlstr_inq);
					cmd.ExecuteReader();
					if (cmd.Read())
					{
						st_seq_no = cmd.GetDecimal(1) + 1;
					}
					ttmsm203["SEQ_NO"] = st_seq_no;		//序号

					ttmsm203.Insert();//插入一条新记录

				}
				else
				{
					sprintf(s.msg, "该条设备记录今日已更换，请选择未更换的记录！");
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


