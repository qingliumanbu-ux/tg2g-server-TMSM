/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 钢包跟踪履历
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包跟踪履历
/// <para>
/// * 记录钢包履历
/// <para>
/// </summary>
/// <param name="LADLE_NO">钢包号</param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件 
  
 
 


int f_tmsm_trace97(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{ 
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");    
	CString	datetemp("");  
	CString	cs_event_desc("");    
	CString	cs_tm_seq_id("");
	CString	date_time = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDecimal stop_use_time = 0;

	/* 实体类定义 */
	CModel ttmsm21("TTMSM21");
	CModel ttmsm31("TTMSM31");
	CModel ttmsm97("TTMSM97");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 判断是否存在指定块 */
		blkNum = bcls_rec->Tables.IndexOf("TM0099");
		if(blkNum < 0)
		{
			strcpy(s.msg,_RES("GCRSS0000007"));/*系统出现异常，请联系系统维护人员。*/
			strcpy(s.sysmsg,"传入数据块TM0099不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		for(int i = 0; i < bcls_rec->Tables["TM0099"].Rows.get_Count(); i++ ) 
		{
			/* 获取输入参数 */
			ttmsm97.MergeFrom(bcls_rec->Tables["TM0099"].Rows[i]);
			//ttmsm97.EQU_TYPE = bcls_rec->Tables["TM0099"].Rows[i]["EQU_TYPE"].ToString().Trim();
			//ttmsm97.EQU_NO = bcls_rec->Tables["TM0099"].Rows[i]["EQU_NO"].ToString().Trim();
			//ttmsm97["EVENT_ID"] = bcls_rec->Tables["TM0099"].Rows[i]["EVENT_ID"].ToString().Trim();
			//ttmsm97["EVENT_DESC"] = bcls_rec->Tables["TM0099"].Rows[i]["EVENT_DESC"].ToString().Trim();
			//ttmsm97["FUNC_ID"] = bcls_rec->Tables["TM0099"].Rows[i]["FUNC_ID"].ToString().Trim();
			//ttmsm97["STATUS"] = bcls_rec->Tables["TM0099"].Rows[i]["STATUS"].ToString().Trim();
			
			/* 检查输入参数合法性 */
			if (ttmsm97["MOLD_TYPE"].ToString().Trim() == "")
			{
				strcpy(s.msg, "设备类型不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (ttmsm97["MOLD_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "设备号不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (ttmsm97["EVENT_ID"].ToString().Trim() == "")
			{
				strcpy(s.msg, "事件号不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			
			
			sqlstr = " select to_char(CURRENT_TIMESTAMP,'YYYYMMDDHH24MISSFF6')  from dual";
			cmd_inq.SetCommandText(sqlstr);
			Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", (const char*)sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				ttmsm97["RESUME_SEQ_NO"] = cmd_inq.GetString(1); /*将数据获取到实体对象中*/
			}
			cmd_inq.Close();
			Log::Trace("", __FUNCTION__, "ttmsm97.RESUME_SEQ_NO =[{0}]", (const char*)ttmsm97["RESUME_SEQ_NO"].ToString());
			ttmsm97["REC_CREATE_TIME"] = date_time;
			ttmsm97["REC_CREATOR"] = s.userid;
			ttmsm97["FORM_NAME"] = s.formname;
			ttmsm97["FUNC_ID"] = s.svc_name;
			ttmsm97.Insert();
 
		} 
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,"数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
} 
