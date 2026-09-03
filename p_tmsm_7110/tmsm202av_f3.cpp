/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-09-27
Description: 设备隐患排查记录维护
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm202av_f3)

int f_tmsm202av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_inq = " ";
	CString msgstr = "提示信息:";	//提示信息
	CString s_userid("");
	CString	datetime("");
	CDateTime accstarttime;
	CDateTime accendtime;
	CModel ttmsm202("TTMSM202");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	int proc_sum = 0;				//操作总数

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());

			Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				if (bcls_rec->Tables["ADD"].Rows[i]["WORK_AREA"].ToString().Trim() == "")
				{
					sprintf(s.msg, "作业区不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["LINE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "产线不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["DEV_NAME"].ToString().Trim() == "")
				{
					sprintf(s.msg, "设备名称不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["SPACE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "部位描述不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["BACK_DESC1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "存在隐患描述不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["BACK_DESC2"].ToString().Trim() == "")
				{
					sprintf(s.msg, "隐患整治措施不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["EXAMINER"].ToString().Trim() == "")
				{
					sprintf(s.msg, "检查人不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["RESP"].ToString().Trim() == "")
				{
					sprintf(s.msg, "整改负责人不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["CHECK_FLAG"].ToString().Trim() == "")
				{
					sprintf(s.msg, "整改与否不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				ttmsm202.Reset();
				ttmsm202.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				//ttmsm202["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
				Log::Trace("", __FUNCTION__, bcls_rec->Tables["ADD"].Rows[0]["DATE_C"].ToString());
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				//if (bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString().Trim() == "")
				//{
				//	ttmsm202["DATE_C"] = datetime.SubstringNE(0, 8);//填表日期为当前日期

				//}
				//else
				//{
				//	ttmsm202["DATE_C"] = bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString().SubstringNE(0, 10).Remove("-");//填表日期为查询日期

				//}
				ttmsm202["DATE_C"] = datetime.SubstringNE(0, 8);
				ttmsm202["REC_CREATOR"] = s.userid;			//记录创建责任者
				ttmsm202["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				//ttmsm202["SEQ_NO"] = bcls_rec->Tables[0].Rows[i]["SEQ_NO"].ToString();//序号
				CDecimal st_seq_no = 1;
				sqlstr_inq = " SELECT MAX(SEQ_NO) FROM TTMSM202";
				Log::Trace("", __FUNCTION__, "str_sql_inq		= [{0}]", sqlstr_inq);
				cmd.SetCommandText(sqlstr_inq);
				cmd.ExecuteReader();
				if (cmd.Read())
				{
					st_seq_no = cmd.GetDecimal(1) + 1;
				}
				ttmsm202["SEQ_NO"] = st_seq_no;		//序号
				ttmsm202.Print();
				proc_sum += ttmsm202.Insert();

				
			}


		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				if (bcls_rec->Tables["UPD"].Rows[i]["WORK_AREA"].ToString().Trim() == "")
				{
					sprintf(s.msg, "作业区不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["LINE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "产线不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["DEV_NAME"].ToString().Trim() == "")
				{
					sprintf(s.msg, "设备名称不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["SPACE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "部位描述不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["BACK_DESC1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "存在隐患描述不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["BACK_DESC2"].ToString().Trim() == "")
				{
					sprintf(s.msg, "隐患整治措施不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["EXAMINER"].ToString().Trim() == "")
				{
					sprintf(s.msg, "检查人不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["RESP"].ToString().Trim() == "")
				{
					sprintf(s.msg, "整改负责人不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["CHECK_FLAG"].ToString().Trim() == "")
				{
					sprintf(s.msg, "整改与否不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				ttmsm202.Reset();
				ttmsm202.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "这里1", "");
				ttmsm202["REC_REVISOR"] = s.userid;			//记录修改责任者
				ttmsm202["REC_REVISE_TIME"] = datetime;		//记录修改时刻

				Log::Trace("", __FUNCTION__, "这里2", "");
				proc_sum += ttmsm202.Update("WORK_AREA,LINE_DESC,DATE_C,DEV_NAME,SPACE_DESC,BACK_DESC1,BACK_DESC2,EXAMINER,RESP,CHECK_FLAG,FINISH_TIME", "DATE_C,SEQ_NO");
				Log::Trace("", __FUNCTION__, "这里3", "");

			}

		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				ttmsm202.Reset();
				ttmsm202.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				//if (model.QueryCount(condition) != 1){
				if (!ttmsm202.Query()){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法删除。", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				sqlstr = "DELETE FROM TTMSM202";
				proc_sum += ttmsm202.Delete();
			}
		}
		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

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


