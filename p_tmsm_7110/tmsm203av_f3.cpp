/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-09-27
Description: 设备周期性维护记录维护
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm203av_f3)

int f_tmsm203av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
				if (bcls_rec->Tables["ADD"].Rows[i]["SPACE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "安装位置不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["DEV_NAME"].ToString().Trim() == "")
				{
					sprintf(s.msg, "设备名称不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["ITEM_NAME"].ToString().Trim() == "")
				{
					sprintf(s.msg, "项目不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["PERD_DD"].ToString().Trim() == "")
				{
					sprintf(s.msg, "周期不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["TIME_1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "上次维护时间不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["SHIFT_CLASS"].ToString().Trim() == "")
				{
					sprintf(s.msg, "班组不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["RESP"].ToString().Trim() == "")
				{
					sprintf(s.msg, "责任人不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["TYPE_CODE1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "类别不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["ADD"].Rows[i]["PERD_DD1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "临期天数不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				ttmsm203.Reset();
				ttmsm203.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				//ttmsm203["DATE_C"] = bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().SubstringNE(0,8);		//填表日期
				Log::Trace("", __FUNCTION__, bcls_rec->Tables["ADD"].Rows[0]["DATE_C"].ToString());
				Log::Trace("", "", "--LINE,count=[{0}]", __LINE__);
				ttmsm203["DATE_C"] = datetime.SubstringNE(0, 8);
				if (ttmsm203["TIME_1"].ToString().Trim() != "" && ttmsm203["PERD_DD"].ToString().Trim() != "")
				{
					time_1 = CDateTime::Parse(bcls_rec->Tables["ADD"].Rows[i]["TIME_1"].ToString().SubstringNE(0, 8));//上次维护时间

					perd_dd = bcls_rec->Tables["ADD"].Rows[i]["PERD_DD"].ToDecimal().ToInt32();
					time_2 = time_1.AddDays(perd_dd); //下次计划时间=上次维护时间+周期天数
					Log::Trace("", __FUNCTION__, "TIME_2= {0}", time_2.ToString("yyyyMMdd"));
					ttmsm203["TIME_2"] = time_2.ToString("yyyyMMdd");
					ttmsm203["TIME_1"] = time_1.ToString("yyyyMMdd");



				}
				else
				{
					ttmsm203["TIME_2"] = " ";
					ttmsm203["TIME_1"] = " ";
				}

				ttmsm203["TIME_3"] = " ";
				if (bcls_rec->Tables["ADD"].Rows[0]["CHECK_FLAG"].ToString().Trim() == "")
				{
					ttmsm203["CHECK_FLAG"] = 0; // //设置更换标志默认值为0
				}
				else
				{
					ttmsm203["CHECK_FLAG"] = bcls_rec->Tables["ADD"].Rows[0]["CHECK_FLAG"].ToString().Trim();
				}
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
				proc_sum += ttmsm203.Insert();


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
				if (bcls_rec->Tables["UPD"].Rows[i]["SPACE_DESC"].ToString().Trim() == "")
				{
					sprintf(s.msg, "安装位置不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["DEV_NAME"].ToString().Trim() == "")
				{
					sprintf(s.msg, "设备名称不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["ITEM_NAME"].ToString().Trim() == "")
				{
					sprintf(s.msg, "项目不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["PERD_DD"].ToString().Trim() == "")
				{
					sprintf(s.msg, "周期不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["TIME_1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "上次维护时间不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["SHIFT_CLASS"].ToString().Trim() == "")
				{
					sprintf(s.msg, "班组不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["RESP"].ToString().Trim() == "")
				{
					sprintf(s.msg, "责任人不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["TYPE_CODE1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "类别不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				if (bcls_rec->Tables["UPD"].Rows[i]["PERD_DD1"].ToString().Trim() == "")
				{
					sprintf(s.msg, "临期天数不能为空！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				ttmsm203.Reset();
				ttmsm203.MergeFrom(bcls_rec->Tables["UPD"].Rows[i]);
				Log::Trace("", __FUNCTION__, "这里1", "");
				if (ttmsm203["CHECK_FLAG"].ToString().Trim() == "0" || ttmsm203["CHECK_FLAG"].ToString().Trim() == "")
				{
					Log::Trace("", __FUNCTION__, "CHECK_FLAG= {0}", ttmsm203["CHECK_FLAG"].ToString().Trim());
					if (ttmsm203["TIME_1"].ToString().Trim() != "" && ttmsm203["PERD_DD"].ToString().Trim() != "")
					{
						time_1 = CDateTime::Parse(bcls_rec->Tables["UPD"].Rows[i]["TIME_1"].ToString().SubstringNE(0, 8));//上次维护时间

						perd_dd = bcls_rec->Tables["UPD"].Rows[i]["PERD_DD"].ToDecimal().ToInt32();
						time_2 = time_1.AddDays(perd_dd); //下次计划时间=上次维护时间+周期天数
						Log::Trace("", __FUNCTION__, "TIME_2= {0}", time_2.ToString("yyyyMMdd"));
						ttmsm203["TIME_2"] = time_2.ToString("yyyyMMdd");
						ttmsm203["TIME_1"] = time_1.ToString("yyyyMMdd");



					}
					else
					{
						ttmsm203["TIME_2"] = " ";
						ttmsm203["TIME_1"] = " ";
					}

					ttmsm203["REC_REVISOR"] = s.userid;			//记录修改责任者
					ttmsm203["REC_REVISE_TIME"] = datetime;		//记录修改时刻


					Log::Trace("", __FUNCTION__, "这里2", "");
					proc_sum += ttmsm203.Update("WORK_AREA,LINE_DESC,DATE_C,DEV_NAME,SPACE_DESC,ITEM_NAME,PERD_DD,TIME_1,TIME_2,SHIFT_CLASS,RESP,TYPE_CODE1,PERD_DD1,BACKC1", "DATE_C,SEQ_NO");
					Log::Trace("", __FUNCTION__, "这里3", "");
				}
				else
				{
					sprintf(s.msg, "已更换设备的记录，不允许修改！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
		}
		
		/************************删除*******************************************/
		if (bcls_rec->Tables.Contains("DEL"))
		{
			Log::Trace("", "", "删除开始,count=[{0}]", bcls_rec->Tables["DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				ttmsm203.Reset();
				ttmsm203.MergeFrom(bcls_rec->Tables["DEL"].Rows[i]);

				//if (model.QueryCount(condition) != 1){
				if (!ttmsm203.Query()){
					////Log::Debug("", __FUNCTION__, "未找到第{0}条记录，无法删除。", i + 1);
					msgstr += msgstr.Format("未找到第%d条记录，无法删除。", i + 1);
					continue;
				}
				if (ttmsm203["CHECK_FLAG"].ToString().Trim() == "0")
				{
					sqlstr = "DELETE FROM TTMSM203";
					proc_sum += ttmsm203.Delete();
					
				}

				else
				{
					sprintf(s.msg, "已更换设备的记录，不允许删除！");
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


