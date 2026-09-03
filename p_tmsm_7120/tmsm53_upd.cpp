/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:行车作业命令修改
**************************************************/
//框架头文件
#include "stdafx.h" 

//业务头文件
  

//外部函数声明

BM2F_ENTERACE(tmsm53_upd)                                         

int f_tmsm53_upd(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	
	/* 实体类定义 */ 
	CModel ttmsm53("TTMSM53");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			CString crane_inst_status("");//行车状态

			ttmsm53.Reset();
			ttmsm53.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm53.TrimOrBlank();
			ttmsm53["REC_REVISOR"]			= s.userid;		//记录创建责任者
			ttmsm53["REC_REVISE_TIME"]		= datetime;		//记录创建时刻
			ttmsm53.TrimOrBlank();
			ttmsm53.Print();
			if (ttmsm53["CRANENO"].ToString().Trim() == "")
			{
				sprintf(s.msg, "请输入行车号!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm53["OPER_PLACE"].ToString().Trim() == "")
			{
				sprintf(s.msg, "请输入行车命令!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm53["OPER_PLACE_FROM"].ToString().Trim() == "")
			{
				sprintf(s.msg, "请输入起始作业点!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm53["OPER_PLACE_DEST"].ToString().Trim() == "")
			{
				sprintf(s.msg, "请输入目的地作业点!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (ttmsm53["CRANE_INST_NO"].ToString().Trim() != "")
			{
				sqlstr = "SELECT CRANE_INST_STATUS FROM TTMSM53 WHERE CRANE_INST_NO='" + ttmsm53["CRANE_INST_NO"].ToString().Trim() + "'";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					crane_inst_status = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
			}
			Log::Info("", __FUNCTION__,"crane_inst_status=[{0}]", crane_inst_status);

			if (strcmp(crane_inst_status, ttmsm53["CRANE_INST_STATUS"]) != 0)
			{
				if (crane_inst_status != "1" && crane_inst_status != "2")
				{
					sprintf(s.msg, "命令已执行，不能修改！");
				}
				if (strcmp(ttmsm53["CRANE_INST_STATUS"], "1") != 0 && strcmp(ttmsm53["CRANE_INST_STATUS"], "2") != 0)
				{
					sprintf(s.msg, "优先级只能是正常1或紧急2！");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			if (ttmsm53["OPER_PLACE"].ToString().Trim() != "")
			{
				sqlstr = "SELECT CODE_DESC_2_CONTENT"
					" FROM TEP0002 "
					" WHERE CODE_CLASS in ('TMCC','TMCG','TMCH','TMCI','TMCL','TMCM','TMCN') "
					" AND CODE = @code";

				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("code", ttmsm53["OPER_PLACE"].ToString().Trim());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					ttmsm53["WT_FLAG"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
			}
			Log::Info("", __FUNCTION__, "wt_flag=[{0}]", ttmsm53["WT_FLAG"].ToString());
			ttmsm53.Update("REC_REVISOR ,REC_REVISE_TIME,CRANE_INST_NO,PONO,CRANENO,LADLE_SLOT_NO,OPER_PLACE,OPER_PLACE_DEST,OPER_PLACE_FROM,TYPE_CODE,CRANE_INST_STATUS,WT_FLAG","CRANE_INST_NO");
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
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
	
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
} 

