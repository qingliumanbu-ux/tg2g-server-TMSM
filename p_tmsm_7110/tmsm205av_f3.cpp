/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wcm
Version:     1.0
Date:        2024-09-27
Description: 设备检修记录维护
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm205av_f3)

int f_tmsm205av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_inq = " ";
	CString msgstr = "提示信息:";	//提示信息
	CString s_userid("");
	CString	datetime("");
	CDateTime errorstarttime;
	CDateTime errorendtime;
	CDateTime planstarttime;
	CDateTime planendtime;
	CModel ttmsm201("TTMSM205");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	int proc_sum = 0;				//操作总数
	CString v_operate = "";
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		if (bcls_rec->Tables[0].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[0].Rows[0]["PRO_DIV"].ToString().Trim();
			Log::Trace("", "", "条件查询v_operate[{0}],", v_operate);
		}
		if (v_operate == "I")
		{
			for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
			{
				/*if (bcls_rec->Tables[0].Rows[i]["RESPONSIBILITY_PLANT_3T"].ToString().Trim() == "")
				{
					sprintf(s.msg, "作业区不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables[0].Rows[i]["LINE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "产线不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}*/

				ttmsm201.Reset();
				ttmsm201.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				//ttmsm201["DATE_C"] = datetime.SubstringNE(0, 8);
				if (ttmsm201["ERROR_START_TIME"].ToString().Trim() != ""&&ttmsm201["ERROR_END_TIME"].ToString().Trim() != "")
				{
					errorstarttime = CDateTime::Parse(bcls_rec->Tables[0].Rows[i]["ERROR_START_TIME"].ToString().SubstringNE(0, 16));//故障开始时间
					errorendtime = CDateTime::Parse(bcls_rec->Tables[0].Rows[i]["ERROR_END_TIME"].ToString().SubstringNE(0, 16));//故障结束时间
					CTimeSpan timespan = errorendtime.Subtract(errorstarttime);
					CDecimal totalMinutes = timespan.TotalMinutes();
					Log::Trace("", __FUNCTION__, "totalMinutes		= [{0}]", totalMinutes);
					if (totalMinutes.Round(0) >= 0)
					{
						if (totalMinutes.Round(0) <= 9999999999)
						{
							ttmsm201["ACT_TIME"] = totalMinutes.Round(0);
						}
						else
						{
							sprintf(s.msg, "实际起止时间间隔太大！");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					else
					{
						ttmsm201["ACT_TIME"] = 0;
					}
				}
				
				if (ttmsm201["PLAN_START_TIME"].ToString().Trim() != ""&&ttmsm201["PLAN_END_TIME"].ToString().Trim() != "")
					{
						planstarttime = CDateTime::Parse(bcls_rec->Tables[0].Rows[i]["PLAN_START_TIME"].ToString().SubstringNE(0, 16));//故障开始时间
						planendtime = CDateTime::Parse(bcls_rec->Tables[0].Rows[i]["PLAN_END_TIME"].ToString().SubstringNE(0, 16));//故障结束时间
						CTimeSpan timespan = planendtime.Subtract(planstarttime);
						CDecimal totalMinutes = timespan.TotalMinutes();
						Log::Trace("", __FUNCTION__, "totalMinutes		= [{0}]", totalMinutes);
						if (totalMinutes.Round(0) >= 0)
						{
							if (totalMinutes.Round(0) <= 9999999999)
							{
								ttmsm201["PLAN_TIME"] = totalMinutes.Round(0);
							}
							else
							{
								sprintf(s.msg, "计划起止时间间隔太大！");
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						else
						{
							ttmsm201["PLAN_TIME"] = 0;
						}
					}
				
					
				CDecimal hotTime = CDecimal::Parse(ttmsm201["HOT_STOP_TIME"].ToString());
					
					if (hotTime.Round(0) > 0)
					{
						if (ttmsm201["LINE_DESC"].ToString().Trim() == "02")
						{
							ttmsm201["CAPA_EFFECT"] = (hotTime/70*185).Round(0);
						}
						else if (ttmsm201["LINE_DESC"].ToString().Trim() == "01")
						{
							ttmsm201["CAPA_EFFECT"] = (hotTime / 45 * 195).Round(0);
						}
						else
						{
							ttmsm201["CAPA_EFFECT"] = 0;
						}
					}
				ttmsm201["REC_CREATOR"] = s.userid;			//记录创建责任者
				//ttmsm201["REPAIR_WRITER"] = s.username;     //录入人员
				ttmsm201["FLAG1"] = 0; //0代表北区记录
				ttmsm201["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				
				CDecimal st_seq_no = 1;
				sqlstr_inq = "SELECT nvl(MAX(SEQ_NO),0) FROM TTMSM205";
				Log::Trace("", __FUNCTION__, "str_sql_inq		= [{0}]", sqlstr_inq);
				cmd.SetCommandText(sqlstr_inq);
				cmd.ExecuteReader();
				if (cmd.Read())
				{
					st_seq_no = cmd.GetDecimal(1) + 1;
				}
				ttmsm201["SEQ_NO"] = st_seq_no;		//序号
				proc_sum += ttmsm201.Insert();
			}
		}
		/************************新增*******************************************/
		if (bcls_rec->Tables.Contains("ADD"))
		{
			Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());

			Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);

			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				/*if (bcls_rec->Tables["ADD"].Rows[i]["RESPONSIBILITY_PLANT_3T"].ToString().Trim() == "")
				{
					sprintf(s.msg, "作业区不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["LINE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "产线不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}*/

			

				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				ttmsm201.Reset();
				ttmsm201.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				//ttmsm201["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
				Log::Trace("", __FUNCTION__, bcls_rec->Tables["ADD"].Rows[0]["DATE_C"].ToString());
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				
				//ttmsm201["DATE_C"] = datetime.SubstringNE(0, 8);
				if (ttmsm201["ERROR_START_TIME"].ToString().Trim() != ""&&ttmsm201["ERROR_END_TIME"].ToString().Trim() != "")
				{
					errorstarttime = CDateTime::Parse(bcls_rec->Tables["ADD"].Rows[i]["ERROR_START_TIME"].ToString().SubstringNE(0, 16));//故障开始时间
					errorendtime = CDateTime::Parse(bcls_rec->Tables["ADD"].Rows[i]["ERROR_END_TIME"].ToString().SubstringNE(0, 16));//故障结束时间
					CTimeSpan timespan = errorendtime.Subtract(errorstarttime);
					CDecimal totalMinutes = timespan.TotalMinutes();
					Log::Trace("", __FUNCTION__, "totalMinutes		= [{0}]", totalMinutes);
					if (totalMinutes.Round(0) >= 0)
					{
						if (totalMinutes.Round(0) <= 9999999999)
						{
							ttmsm201["ACT_TIME"] = totalMinutes.Round(0);
						}
						else
						{
							sprintf(s.msg, "实际起止时间间隔太大！");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					else
					{
						ttmsm201["ACT_TIME"] = 0;
					}
				}
				
				if (ttmsm201["PLAN_START_TIME"].ToString().Trim() != ""&&ttmsm201["PLAN_END_TIME"].ToString().Trim() != "")
				  {
					planstarttime = CDateTime::Parse(bcls_rec->Tables["ADD"].Rows[i]["PLAN_START_TIME"].ToString().SubstringNE(0, 16));//故障开始时间
					planendtime = CDateTime::Parse(bcls_rec->Tables["ADD"].Rows[i]["PLAN_END_TIME"].ToString().SubstringNE(0, 16));//故障结束时间
					CTimeSpan timespan = planendtime.Subtract(planstarttime);
					CDecimal totalMinutes = timespan.TotalMinutes();
					Log::Trace("", __FUNCTION__, "totalMinutes		= [{0}]", totalMinutes);
					if (totalMinutes.Round(0) >= 0)
					{
						if (totalMinutes.Round(0) <= 9999999999)
						{
							ttmsm201["PLAN_TIME"] = totalMinutes.Round(0);
						}
						else
						{
							sprintf(s.msg, "计划起止时间间隔太大！");
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					else
					{
						ttmsm201["PLAN_TIME"] = 0;
					}
				  }
				CDecimal hotTime = CDecimal::Parse(ttmsm201["HOT_STOP_TIME"].ToString());
				if (hotTime.Round(0) > 0)
				{
					if (ttmsm201["LINE_DESC"].ToString().Trim() == "02")
					{
						ttmsm201["CAPA_EFFECT"] = (hotTime / 70 * 185).Round(0);
					}
					else if (ttmsm201["LINE_DESC"].ToString().Trim() == "01")
					{
						ttmsm201["CAPA_EFFECT"] = (hotTime / 45 * 195).Round(0);
					}
					else
					{
						ttmsm201["CAPA_EFFECT"] = 0;
					}
				}
				ttmsm201["REC_CREATOR"] = s.userid;			//记录创建责任者
				//ttmsm201["REPAIR_WRITER"] = s.username;     //录入人员
				ttmsm201["FLAG1"] = 0; //0代表北区记录
				ttmsm201["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				//ttmsm201["SEQ_NO"] = bcls_rec->Tables[0].Rows[i]["SEQ_NO"].ToString();//序号
				CDecimal st_seq_no = 1;
				sqlstr_inq = "SELECT nvl(MAX(SEQ_NO),0) FROM TTMSM205";
				Log::Trace("", __FUNCTION__, "str_sql_inq		= [{0}]", sqlstr_inq);
				cmd.SetCommandText(sqlstr_inq);
				cmd.ExecuteReader();
				if (cmd.Read())
				{
					st_seq_no = cmd.GetDecimal(1) + 1;
				}
				ttmsm201["SEQ_NO"] = st_seq_no;		//序号
				proc_sum += ttmsm201.Insert();
			}

		}

		/************************修改*******************************************/
		if (bcls_rec->Tables.Contains("UPD"))
		{
			Log::Trace("", "", "修改开始,count=[{0}]", bcls_rec->Tables["UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["UPD"].Rows.get_Count(); i++)
			{
				if (bcls_rec->Tables["UPD"].Rows[i]["FLAG1"].ToString().Trim() == "0") //作业区可以修改作业区记录
				{
					/*if (bcls_rec->Tables["UPD"].Rows[i]["RESPONSIBILITY_PLANT_3T"].ToString().Trim() == "")
					{
						sprintf(s.msg, "作业区不能为空！");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
					if (bcls_rec->Tables["UPD"].Rows[i]["LINE_DESC"].ToString().Trim() == "")
					{
						sprintf(s.msg, "产线不能为空！");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}*/
					ttmsm201.Reset();
					ttmsm201.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
					Log::Trace("", __FUNCTION__, "这里1", "");

					if (ttmsm201["ERROR_START_TIME"].ToString().Trim() != ""&&ttmsm201["ERROR_END_TIME"].ToString().Trim() != "")
					{
						errorstarttime = CDateTime::Parse(bcls_rec->Tables["UPD"].Rows[i]["ERROR_START_TIME"].ToString().SubstringNE(0, 16));//故障开始时间
						errorendtime = CDateTime::Parse(bcls_rec->Tables["UPD"].Rows[i]["ERROR_END_TIME"].ToString().SubstringNE(0, 16));//故障结束时间
						CTimeSpan timespan = errorendtime.Subtract(errorstarttime);
						CDecimal totalMinutes = timespan.TotalMinutes();
						Log::Trace("", __FUNCTION__, "totalMinutes		= [{0}]", totalMinutes);
						if (totalMinutes.Round(0) >= 0)
						{
							if (totalMinutes.Round(0) <= 9999999999)
							{
							  ttmsm201["ACT_TIME"] = totalMinutes.Round(0);
							}
							else
							{
								sprintf(s.msg, "实际起止时间间隔太大！");
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						else
						{
							ttmsm201["ACT_TIME"] = 0;
						}
					}
					
					if (ttmsm201["PLAN_START_TIME"].ToString().Trim() != ""&&ttmsm201["PLAN_END_TIME"].ToString().Trim() != "")
						{
							planstarttime = CDateTime::Parse(bcls_rec->Tables["UPD"].Rows[i]["PLAN_START_TIME"].ToString().SubstringNE(0, 16));//故障开始时间
							planendtime = CDateTime::Parse(bcls_rec->Tables["UPD"].Rows[i]["PLAN_END_TIME"].ToString().SubstringNE(0, 16));//故障结束时间
							CTimeSpan timespan = planendtime.Subtract(planstarttime);
							CDecimal totalMinutes = timespan.TotalMinutes();
							Log::Trace("", __FUNCTION__, "totalMinutes		= [{0}]", totalMinutes);
							if (totalMinutes.Round(0) >= 0)
							{
								if (totalMinutes.Round(0) <= 9999999999)
								{
									ttmsm201["PLAN_TIME"] = totalMinutes.Round(0);
								}
								else
								{
									sprintf(s.msg, "计划起止时间间隔太大！");
									throw CApplicationException(-1, s.msg, s.svc_name);
								}
							}
							else
							{
								ttmsm201["PLAN_TIME"] = 0;
							}
						}
					CDecimal hotTime = CDecimal::Parse(ttmsm201["HOT_STOP_TIME"].ToString());
					if (hotTime.Round(0) > 0)
					{
						if (ttmsm201["LINE_DESC"].ToString().Trim() == "02")
						{
							ttmsm201["CAPA_EFFECT"] = (hotTime / 70 * 185).Round(0);
						}
						else if (ttmsm201["LINE_DESC"].ToString().Trim() == "01")
						{
							ttmsm201["CAPA_EFFECT"] = (hotTime / 45 * 195).Round(0);
						}
						else
						{
							ttmsm201["CAPA_EFFECT"] = 0;
						}
					}

					ttmsm201["REC_REVISOR"] = s.userid;			//记录修改责任者
					//ttmsm201["REPAIR_WRITER"] = s.username;     //录入人员
					ttmsm201["REC_REVISE_TIME"] = datetime;		//记录修改时刻
					Log::Trace("", __FUNCTION__, "这里2", "");
					proc_sum += ttmsm201.Update("DATE_C,LINE_DESC,DEV_NAME,PLACE_N,ABNR_TYP,JX_ITEM_DESC,PLAN_START_TIME,PLAN_END_TIME,ERROR_START_TIME,ERROR_END_TIME,PLAN_TIME,ACT_TIME,HOT_STOP_TIME,CAPA_EFFECT,RESPONSIBILITY_PLANT_3T,RESP,SHIFT_GROUP_ZR,TYPE_CODE1,REPAIR_WRITER,C_ISCALL_ZD,BACKC1", "SEQ_NO");
					Log::Trace("", __FUNCTION__, "这里3", "");


				}
				



			}
		}

		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				ttmsm201.Reset();
				ttmsm201.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				//if (model.QueryCount(condition) != 1){
				if (!ttmsm201.Query()){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法删除。", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				if (bcls_rec->Tables["DEL"].Rows[i]["FLAG1"].ToString().Trim() == "0") //北区可以删除
				{
					sqlstr = "DELETE FROM TTMSM205";
					proc_sum += ttmsm201.Delete();
				}
				else if (bcls_rec->Tables["DEL"].Rows[i]["FLAG1"].ToString().Trim() == "1") //南区不可以删除
				{

					sprintf(s.msg, "南区记录不能删除！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}


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


