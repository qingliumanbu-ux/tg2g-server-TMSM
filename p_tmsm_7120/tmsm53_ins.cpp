/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     张利锋
Version:    1.0
Date:       2023-05-19
Description:行车作业命令新增
**************************************************/
//框架头文件
#include "stdafx.h" 


//业务头文件
  

//外部函数声明
int EPGetNextSeq(const char* seq_name, char* tcseq);

BM2F_ENTERACE(tmsm53_ins)                                         

int f_tmsm53_ins(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString	oper_place("");
	CString	cast_inst_no("");//行车命令号流水号
	CString	crane_inst_seq("");//行车请求序号
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
			if (bcls_rec->Tables[0].Columns.Contains("OPER_PLACE"))
			{
				oper_place = bcls_rec->Tables[0].Rows[i]["OPER_PLACE"].ToString().Trim();
			}
			ttmsm53.Reset();
			ttmsm53.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm53.TrimOrBlank();
			

			ttmsm53["REC_CREATOR"]			= s.userid;		//记录创建责任者
			ttmsm53["REC_CREATE_TIME"]		= datetime;		//记录创建时刻
			ttmsm53.TrimOrBlank();
			ttmsm53.Print();


			/* 查询该钢包号是否存在 */
			if (ttmsm53.QueryCount("CRANE_INST_NO") > 0)
			{
				sprintf(s.msg, "行车命令号[%s]已存在", (const char*)ttmsm53["CRANE_INST_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			
			if (ttmsm53["CRANENO"].ToString() =="")
			{
				sprintf(s.msg, "请输入行车号");
				throw CApplicationException(-1, s.msg, s.svc_name);
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
				ttmsm53["STRIDE_CODE"] = oper_place.Substring(0, 1);

				if (oper_place.Substring(0, 1) = "6")
				{
					crane_inst_seq = EPGetNextSeq("CRANE_INST_NO", conn);
				}
				if (oper_place.Substring(0, 1) = "7")
				{
					crane_inst_seq = EPGetNextSeq("CRANE_INST_NO_2", conn);
				}
				if (oper_place.Substring(0, 1) = "8")
				{
					crane_inst_seq = EPGetNextSeq("CRANE_INST_NO_3", conn);
				}
				sqlstr = "select '" + oper_place.Substring(0, 1) + "'||lpad('" + crane_inst_seq + "', 6, '0') from sysibm.dual";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					ttmsm53["CRANE_INST_NO"] = cmd_inq.GetString(1);
					ttmsm53["CRANE_INST_SEQ"] = crane_inst_seq;
				}
			}
			Log::Info("", __FUNCTION__, "wt_flag=[{0}]", ttmsm53["WT_FLAG"].ToString());
			
			ttmsm53.Print();
			ttmsm53.Insert();
			
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

