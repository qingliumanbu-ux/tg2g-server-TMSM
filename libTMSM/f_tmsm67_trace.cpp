/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-31 14:45:52
Description: 其它工器具跟踪履历
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm67("TTMSM67");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/*判断是否存在指定块*/

		blkNum = bcls_rec->Tables.IndexOf("TMSM67");
		if (blkNum < 0)
		{
			strcpy(s.sysmsg, "传入数据块TMSM67不存在。");
			strcpy(s.msg, _RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		/* 获取输入参数 */
		for (int i = 0; i < bcls_rec->Tables["TMSM67"].Rows.get_Count(); i++)
		{
			ttmsm67.Reset();

			/*  以下写履历表*/
			ttmsm67["REC_CREATOR"] = s.userid;							// 记录创建责任者, ,
			ttmsm67["REC_CREATE_TIME"] = datetime;						//记录创建时刻
			ttmsm67["REC_REVISOR"] = s.userid;							//记录修改责任者
			ttmsm67["REC_REVISE_TIME"] = datetime;						//记录修改时刻
			ttmsm67["ARCHIVE_FLAG"] = " ";								//归档标记   
			ttmsm67["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_95", conn));	//序号
			//ttmsm67["SM_UNIT_NO"] = " ";								//炼钢单元号
			ttmsm67["SM_UNIT_NO"] = bcls_rec->Tables["TMSM67"].Rows[i]["SM_UNIT_NO"].ToString();


			ttmsm67["CC_MACH_NO"] = " ";								//连铸机号
			ttmsm67["STRAND_NO"] = " ";									//流号
			ttmsm67["DEV_NO"] = " ";									//设备编号_001
			ttmsm67["DEV_NAME"] = " ";									//设备名称
			ttmsm67["CHANGE_TIME"] = " ";								//变动时间
			ttmsm67["CHANGE_RES"] = " ";								//更换原因代码
			ttmsm67["CHANGE_RES_C"] = " ";								//更换原因描述
			ttmsm67["INSPECT_TIME_CC"] = " ";							//铸机检测时间
			ttmsm67["INSPECT_CONCL_CC"] = " ";							//铸机检测结论
			ttmsm67["INSPECT_TIME_NOZZLE"] = " ";						//喷嘴检测时间
			ttmsm67["INSPECT_CONCL_NOZZLE"] = " ";						//喷嘴检测结论
			ttmsm67["REMARK_2"] = " ";									//备注2
			//ttmsm67["REMARK_1"] = " ";									//备注1
			ttmsm67["REMARK_1"] = bcls_rec->Tables["TMSM67"].Rows[i]["REMARK_1"].ToString();

			ttmsm67["REMARK_3"] = s.svc_name;					//后台程序名
			ttmsm67["REMARK_4"] = s.formname;					//前台画面名
			ttmsm67["REMARK_5"] = s.fore_ip;					//前台IP地址
			ttmsm67["REMARK_6"] = s.fore_machine;				//前台主机名称

			ttmsm67["REMARK_7"] = "	";						
			ttmsm67["REMARK_8"] = "	";				
			//ttmsm67["REMARK"]   = " ";							//备注
			ttmsm67["REMARK"] = bcls_rec->Tables["TMSM67"].Rows[i]["REMARK"].ToString();


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


