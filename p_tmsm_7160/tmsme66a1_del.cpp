/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-23 10:21:49
Description: 扇形段跟踪信息删除
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme66a1_del)


int f_tmsme66a1_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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


		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
			Log::Trace("", __FUNCTION__, "datetime		= [{0}]", datetime);
			/* ***** 获取输入参数 ***** */
			if (bcls_rec->Tables[0].Columns.Contains("SM_UNIT_NO"))
				ttmsm66["SM_UNIT_NO"] = bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"];
			if (bcls_rec->Tables[0].Columns.Contains("DEV_NO"))
				ttmsm66["DEV_NO"] = bcls_rec->Tables[0].Rows[0]["DEV_NO"];
			if (bcls_rec->Tables[0].Columns.Contains("CC_MACH_NO"))
				ttmsm66["CC_MACH_NO"] = bcls_rec->Tables[0].Rows[0]["CC_MACH_NO"];
			if (bcls_rec->Tables[0].Columns.Contains("STRAND_NO"))
				ttmsm66["STRAND_NO"] = bcls_rec->Tables[0].Rows[0]["STRAND_NO"];

			Log::Trace("", __FUNCTION__, "ttmsm66.SM_UNIT_NO		= [{0}]", (const char*)ttmsm66["SM_UNIT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.DEV_NO		= [{0}]", (const char*)ttmsm66["DEV_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.CC_MACH_NO		= [{0}]", (const char*)ttmsm66["CC_MACH_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.STRAND_NO		= [{0}]", (const char*)ttmsm66["STRAND_NO"].ToString());

			/* 检查输入参数合法性 */
			if (ttmsm66["SM_UNIT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "厂别区分不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm66["DEV_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "设备编号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm66["CC_MACH_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "连铸机号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm66["STRAND_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "流号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			/* 删除工器具基本信息表信息 */
			ttmsm66.Delete("SM_UNIT_NO,CC_MACH_NO,STRAND_NO,DEV_NO");

			//Log::Trace("", "", "sqlstr：{0}", sqlstr);

			//写67履历表
			ttmsm67["REC_CREATOR"] = s.userid;							// 记录创建责任者, ,
			ttmsm67["REC_CREATE_TIME"] = datetime;						//记录创建时刻
			ttmsm67["REC_REVISOR"] = s.userid;							//记录修改责任者
			ttmsm67["REC_REVISE_TIME"] = datetime;						//记录修改时刻
			ttmsm67["ARCHIVE_FLAG"] = " ";								//归档标记   
			ttmsm67["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_95", conn));	//序号
			ttmsm67["SM_UNIT_NO"] = ttmsm66["SM_UNIT_NO"];				//炼钢单元号

			ttmsm67["CC_MACH_NO"] = ttmsm66["SM_UNIT_NO"];				//连铸机号
			ttmsm67["STRAND_NO"] = ttmsm66["STRAND_NO"];				//流号
			ttmsm67["DEV_NO"] = ttmsm66["DEV_NO"];						//设备编号_001
			ttmsm67["DEV_NAME"] = ttmsm66["DEV_NAME"];					//设备名称
			ttmsm67["CHANGE_TIME"] = ttmsm66["SM_UNIT_NO"];				//变动时间
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
			ttmsm67["REMARK_7"] = "tmsme66a1_del";						//ttmsm66["REMARK_7"];
			ttmsm67["REMARK_8"] = "工器具扇形段跟踪信息删除";			//ttmsm66["REMARK_8"];
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

