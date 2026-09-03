/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      KE2163
Version:     1.0
Date:        2023-01-12 15:49:45
Description: 行车命令状态变动
**************************************************/

#include "stdafx.h"

//BM2_FUNCTION_EXPORT

int f_tmsm53_status_chg(EIClass* bcls_rec, EIClass* bcls_ret)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";

	CString dataTime = "";

	//规则引擎传入参数
	CString crane_inst_no = "";		//行车命令号
	CString crane_inst_status = "";			//行车命令状态

	//实绩值写表
	CString rec_creator = "";		//创建者
	CString rec_create_time = "";	//创建时间

	int count = 0;
	int length = 0;

	CModel ttmsm52("TTMSM52");
	CModel ttmsm53("TTMSM53");
	CModel ttmsm54("TTMSM54");


	try
	{
		EDLog(1, 1, " **************%s Begin *****************", "f_tmsm53_status_chg");		EIClass inBlock;
		EIClass outBlock;

		dataTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		/* 获得传入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[0].Columns.Contains("CRANE_INST_NO"))
			{
				crane_inst_no = bcls_rec->Tables[0].Rows[0]["CRANE_INST_NO"].ToString();
			}
			if (bcls_rec->Tables[0].Columns.Contains("CRANE_INST_STATUS"))
			{
				crane_inst_status = bcls_rec->Tables[0].Rows[0]["CRANE_INST_STATUS"].ToString();
			}
			Log::Info("", __FUNCTION__, "crane_inst_no=[{0}]", crane_inst_no);
			Log::Info("", __FUNCTION__, "crane_inst_status=[{0}]", crane_inst_status);
			ttmsm53.Reset();
			//ttmsm53.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm53["CRANE_INST_NO"] = crane_inst_no;
			ttmsm53.TrimOrBlank();
			if (!ttmsm53.Query("CRANE_INST_NO"))
			{
				sprintf(s.msg, "命令号[%s不存在！", (const char*)ttmsm53["CRANE_INST_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (crane_inst_status.Trim() == "5" && ttmsm53["CRANE_INST_STATUS"].ToString().Trim() != "4")
			{
				sprintf(s.msg, "命令未吊起，不能卸下！");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			//ttmsm53.Query("CRANE_INST_NO");
			ttmsm53["REC_REVISE_TIME"] = dataTime;
			ttmsm53["REC_REVISOR"] = s.userid; 
			ttmsm53["CRANE_INST_STATUS"] = crane_inst_status;
			if (crane_inst_status.Trim() == "4")
				ttmsm53["SUSPEND_TIME"] = dataTime;
			if (crane_inst_status.Trim() == "5")
				ttmsm53["D_TIME"] = dataTime;
			ttmsm53.Print();
			ttmsm53.Update("REC_REVISOR,REC_REVISE_TIME,SUSPEND_TIME,D_TIME,CRANE_INST_STATUS","CRANE_INST_NO");
			

			/*行车状态跟踪*/ 
			if (crane_inst_status.Trim()== '4')
			{
				EDLog(1, 1, "行车状态跟踪");
				ttmsm52["REC_REVISOR"] = ttmsm53["REC_REVISOR"];
				ttmsm52["REC_REVISE_TIME"] = ttmsm53["REC_REVISE_TIME"];
				ttmsm52["CRANE_INST_NO"] = ttmsm53["CRANE_INST_NO"];
				ttmsm52["OPER_PLACE"] = ttmsm53["OPER_PLACE_FROM"];
				ttmsm52["PONO"] = ttmsm53["PONO"];
				ttmsm52["LADLE_SLOT_NO"] = ttmsm53["LADLE_SLOT_NO"];
				ttmsm52["CRANE_STAT"] = "1";
				ttmsm52["CRANENO"] = ttmsm53["CRANENO"];
				ttmsm52.Update("REC_REVISOR,REC_REVISE_TIME,CRANE_INST_NO,OPER_PLACE,PONO,LADLE_SLOT_NO,CRANE_STAT","CRANENO");

			}
			 
			if (crane_inst_status.Trim() == '5')
			{
				EDLog(1, 1, "行车状态跟踪");
				ttmsm52["REC_REVISOR"] = ttmsm53["REC_REVISOR"];
				ttmsm52["REC_REVISE_TIME"] = ttmsm53["REC_REVISE_TIME"];
				ttmsm52["CRANE_INST_NO"] = ttmsm53["CRANE_INST_NO"];
				ttmsm52["OPER_PLACE"] = ttmsm53["OPER_PLACE_DEST"];
				ttmsm52["PONO"] = ttmsm53["PONO"];
				ttmsm52["LADLE_SLOT_NO"] = ttmsm53["LADLE_SLOT_NO"];
				ttmsm52["CRANE_STAT"] = "0";/*行车状态:使用中*/
				ttmsm52["CRANENO"] = ttmsm53["CRANENO"];
				ttmsm52.Update("REC_REVISOR,REC_REVISE_TIME,CRANE_INST_NO,OPER_PLACE,PONO,LADLE_SLOT_NO,CRANE_STAT", "CRANENO");
			}

			ttmsm52.Print();
			/*行车命令状态是删除或卸下时归档*/
			if (crane_inst_status.Trim() == '3' || crane_inst_status.Trim() == '5')
			{
				EDLog(1, 1, "行车命令状态是删除或卸下时归档");
				ttmsm54.CopyFrom(ttmsm53);
				ttmsm54.Insert();
				ttmsm53.Delete("CRANE_INST_NO");
			}
			ttmsm54.Print();
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


