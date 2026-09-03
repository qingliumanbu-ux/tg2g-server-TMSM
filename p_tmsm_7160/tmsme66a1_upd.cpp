/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-23 10:20:32
Description: 扇形段跟踪信息修改
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme66a1_upd)


int f_tmsme66a1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm66 = CModel("TTMSM66");
	CModel ttmsm67("TTMSM67");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm66.Reset();
			ttmsm66.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm66.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "ttmsm66.DEV_NO		= [{0}]", (const char*)ttmsm66["DEV_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.SM_UNIT_NO	= [{0}]", (const char*)ttmsm66["SM_UNIT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.CC_MACH_NO	= [{0}]", (const char*)ttmsm66["CC_MACH_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.STRAND_NO		= [{0}]", (const char*)ttmsm66["STRAND_NO"].ToString());

			if (ttmsm66["DEV_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "设备编码不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询该钢包号是否存在 */
			if (ttmsm66.QueryCount("DEV_NO") <= 0)
			{
				sprintf(s.msg, "设备编码[%s]不存在!", (const char*)ttmsm66["DEV_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//------------------------------------------------------------------
			//ttmsm91["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_91", conn));

			/* 修改工器具基本信息表 */
			ttmsm66["REC_REVISOR"] = s.userid;   //记录修改责任者
			ttmsm66["REC_REVISE_TIME"] = datetime;   //记录修改时刻

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME66' AND CODE_CLASS='CHANGE_RES_C' AND CODE = @change_res";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("change_res", ttmsm66["CHANGE_RES"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm66["CHANGE_RES_C"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm66["CHANGE_RES_C"] = " ";
			}
			cmd_inq.Close();

			sqlstr = "UPDATE TTMSM66 SET REC_REVISOR= '" + ttmsm66["REC_REVISOR"].ToString() + "',"
				"REC_REVISE_TIME = '" + ttmsm66["REC_REVISE_TIME"].ToString() + "',"
				"DEV_NAME= '" + ttmsm66["DEV_NAME"].ToString() + "',"					/*设备名称 设备号*/
				"CHANGE_TIME= '" + ttmsm66["CHANGE_TIME"].ToString() + "',"				//变动时间
				"CHANGE_RES= '" + ttmsm66["CHANGE_RES"].ToString() + "',"				//更换原因代码
				"CHANGE_RES_C= '" + ttmsm66["CHANGE_RES_C"].ToString() + "',"			//更换原因描述
				"INSPECT_TIME_CC= '" + ttmsm66["INSPECT_TIME_CC"].ToString() + "',"		//铸机检测时间
				"INSPECT_CONCL_CC= '" + ttmsm66["INSPECT_CONCL_CC"].ToString() + "',"				//铸机检测结论
				"INSPECT_TIME_NOZZLE= '" + ttmsm66["INSPECT_TIME_NOZZLE"].ToString() + "',"			//喷嘴检测时间
				"INSPECT_CONCL_NOZZLE = '" + ttmsm66["INSPECT_CONCL_NOZZLE"].ToString() + "',"		//喷嘴检测结论
				"REMARK_7= 'tmsme66a1_upd',"														/*操作类型*/
				"REMARK_8= '" + ttmsm66["REMARK_8"].ToString() + "' "								/*工器具扇形段跟踪信息修改*/
				" WHERE SM_UNIT_NO = '" + ttmsm66["SM_UNIT_NO"].ToString() + "' AND DEV_NO = '" + ttmsm66["DEV_NO"].ToString() + "'"
				" AND CC_MACH_NO = '" + ttmsm66["CC_MACH_NO"].ToString() + "' AND STRAND_NO = '" + ttmsm66["STRAND_NO"].ToString() + "'";

			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::Execute(sqlstr);

			/*  以下写履历表*/
			ttmsm67["REC_CREATOR"] = s.userid;							// 记录创建责任者, ,
			ttmsm67["REC_CREATE_TIME"] = datetime;						//记录创建时刻
			ttmsm67["REC_REVISOR"] = s.userid;							//记录修改责任者
			ttmsm67["REC_REVISE_TIME"] = datetime;						//记录修改时刻
			ttmsm67["ARCHIVE_FLAG"] = " ";								//归档标记   
			ttmsm67["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_66", conn));	//序号
			ttmsm67["SM_UNIT_NO"] = ttmsm66["SM_UNIT_NO"];				//炼钢单元号

			ttmsm67["CC_MACH_NO"] = ttmsm66["SM_UNIT_NO"];				//连铸机号
			ttmsm67["STRAND_NO"] = ttmsm66["STRAND_NO"];				//流号
			ttmsm67["DEV_NO"] = ttmsm66["DEV_NO"];						//设备编号_001
			ttmsm67["DEV_NAME"] = ttmsm66["DEV_NAME"];					//设备名称
			ttmsm67["CHANGE_TIME"] = ttmsm66["CHANGE_TIME"];				//变动时间
			ttmsm67["CHANGE_RES"] = ttmsm66["CHANGE_RES"];				//更换原因代码
			ttmsm67["CHANGE_RES_C"] = ttmsm66["CHANGE_RES_C"];			//更换原因描述
			ttmsm67["INSPECT_TIME_CC"] = ttmsm66["INSPECT_TIME_CC"];	//铸机检测时间
			ttmsm67["INSPECT_CONCL_CC"] = ttmsm66["INSPECT_CONCL_CC"]; //铸机检测结论
			ttmsm67["INSPECT_TIME_NOZZLE"] = ttmsm66["INSPECT_TIME_NOZZLE"]; //喷嘴检测时间
			ttmsm67["INSPECT_CONCL_NOZZLE"] = ttmsm66["INSPECT_CONCL_NOZZLE"];//喷嘴检测结论
			ttmsm67["REMARK_2"] = ttmsm66["REMARK_2"];					//备注2'
			ttmsm67["REMARK_1"] = ttmsm66["REMARK_1"];					//备注1'
			ttmsm67["REMARK_3"] = ttmsm66["REMARK_3"];					//备注3'
			ttmsm67["REMARK_4"] = ttmsm66["REMARK_4"];
			ttmsm67["REMARK_6"] = ttmsm66["REMARK_6"];
			ttmsm67["REMARK_5"] = ttmsm66["REMARK_5"];
			ttmsm67["REMARK_7"] = "tmsme66a1_upd";						//ttmsm66["REMARK_7"];
			ttmsm67["REMARK_8"] = "工器具扇形段跟踪信息修改";			//ttmsm66["REMARK_8"];
			ttmsm67["REMARK"] = ttmsm66["REMARK"];						//备注'
			ttmsm67.TrimOrBlank();
			ttmsm67.Insert();

		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


