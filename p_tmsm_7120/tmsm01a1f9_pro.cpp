/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-7-01
Description: 钢包信息维修
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包信息维修
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明
void f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn); //班次班别函数
int f_tmsm_trace97(EIClass *bcls_rec,EIClass *bcls_ret,CDbConnection * conn);

BM2F_ENTERACE(tmsm01a1f9_pro)                                                     

int f_tmsm01a1f9_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString strShiftNo("");
	CString strShiftGroup("");

	/* 实体类定义 */ 
	CModel ttmsm01("TTMSM01");
	CModel ttmsm97("TTMSM97");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		blkNum = bcls_rec->Tables.IndexOf("TM0099"); 
		if (blkNum <= 0)
		{ 
			bcls_rec->Tables.Add("TM0099");		
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"LADLE_NO");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"EVENT_ID");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"EVENT_DESC");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"FUNC_ID");
			bcls_rec->Tables["TM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
		}

		blkNum = bcls_rec->Tables.IndexOf("TM0099");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("TM0099");
		}
		bcls_rec->Tables["TM0099"].Rows.Clear();

		/* 获得传入参数 */
		ttmsm01.Reset();
		ttmsm01.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		ttmsm01.TrimOrBlank();

		ttmsm01["SM_UNIT_NO"]      = "A";

		Log::Trace("",__FUNCTION__,"ttmsm01.SM_UNIT_NO			= [{0}]",(const char*)ttmsm01["SM_UNIT_NO"].ToString());
		Log::Trace("",__FUNCTION__,"ttmsm01.LADLE_NO			= [{0}]",(const char*)ttmsm01["LADLE_NO"].ToString());
		Log::Trace("",__FUNCTION__,"ttmsm01.REPAIR_START_TIME	= [{0}]",(const char*)ttmsm01["REPAIR_START_TIME"].ToString());
		Log::Trace("",__FUNCTION__,"ttmsm01.SCRAP_TIME			= [{0}]",(const char*)ttmsm01["SCRAP_TIME"].ToString());
		Log::Trace("",__FUNCTION__,"ttmsm01.SCRAP_MAKER			= [{0}]",(const char*)ttmsm01["SCRAP_MAKER"].ToString());

		/* 检查输入参数合法性 */
		/*if(ttmsm01["SM_UNIT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"炼钢单元号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		if(ttmsm01["LADLE_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,"钢包号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		if(ttmsm01["REPAIR_START_TIME"].ToString().Trim() == "")
		{
			strcpy(s.msg,"维修开始时刻不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 查询该钢包号是否存在 */
		if(ttmsm01.QueryCount("SM_UNIT_NO,LADLE_NO") <= 0)
		{
			sprintf(s.msg,"钢包号[%s]不存在!",(const char*)ttmsm01["LADLE_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* 查询钢包状态和流水号 */			 
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr = " SELECT LADLE_STATUS, "
					     "		  REPAIR_SEQ_NO "
						 "   FROM TTMSM01 "
						 "  WHERE LADLE_NO 		= @ttmsm01.LADLE_NO ";		
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO",ttmsm01["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm01.LADLE_NO",ttmsm01["LADLE_NO"].ToString());	
		cmd_inq.ExecuteReader();		
		if(cmd_inq.Read())
		{
			ttmsm01["LADLE_STATUS"] = cmd_inq.GetString(1);
			ttmsm01["REPAIR_SEQ_NO"]  = cmd_inq.GetInt32(2);
		}
		cmd_inq.Close();

		/* 钢包状态不是烘烤,新增维修流水号 */
		//if(ttmsm01["LADLE_STATUS"].ToString().Trim() != "31")	//31-维修
		//{
			/* 生成维修流水号REPAIR_SEQ_NO 需按项目组定制 */
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT NVL(MAX(REPAIR_SEQ_NO),0) "
							 "   FROM TTMSM01 "
							 "  WHERE SM_UNIT_NO	= @ttmsm01.SM_UNIT_NO "
							 "    AND LADLE_NO 		= @ttmsm01.LADLE_NO "		
							 "    AND LADLE_LIFE	= @ttmsm01.LADLE_LIFE ";		
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO",ttmsm01["SM_UNIT_NO"].ToString());
			cmd_inq.Parameters.Set("ttmsm01.LADLE_NO",ttmsm01["LADLE_NO"].ToString());	
			cmd_inq.Parameters.Set("ttmsm01.LADLE_LIFE",ttmsm01["LADLE_LIFE"].ToDecimal());	
			cmd_inq.ExecuteReader();		
			if(cmd_inq.Read())
			{
				ttmsm01["REPAIR_SEQ_NO"] = cmd_inq.GetInt32(1) + 1;
			}
			cmd_inq.Close();
		//}
		//else
		//{
		//	/* 钢包状态是维修,维修流水号不变 */
		//	ttmsm01.REPAIR_SEQ_NO	= ttmsm01["REPAIR_SEQ_NO"];
		//}

		/* 设置默认值 */
		ttmsm01["LADLE_STATUS"]	= "31";	//31-维修
		f_epep_get_shift_group("SM",datetime,strShiftNo,strShiftGroup,conn);//取得班次班组
		if(ttmsm01["REPAIR_SHIFT_NO"].ToString().Trim() == "")
		{
			ttmsm01["REPAIR_SHIFT_NO"]		  = strShiftNo;		//生产班次号
		}
		if(ttmsm01["REPAIR_GRP_NO"].ToString().Trim()   == "")
		{
			ttmsm01["REPAIR_GRP_NO"]		  = strShiftGroup;		//生产班次号
		}

		if(ttmsm01["REPAIR_WRITER"].ToString().Trim()   == "")
		{
			ttmsm01["REPAIR_WRITER"]         = s.userid;
		}

		/* 修改钢包信息 */		
		ttmsm01["REC_REVISOR"]		= s.userid;   //记录创建责任者
		ttmsm01["REC_REVISE_TIME"]	= datetime;   //记录创建时刻
		ttmsm01.TrimOrBlank();
		ttmsm01.Print();
		sqlstr = "LADLE_STATUS,"
				 "REPAIR_SEQ_NO,"
				 "REPAIR_POS,"
				 "MAINT_TYPE,"
				 "SMALL_REPAIR_CASE,"
				 //"SMALL_REPAIR_NUM,"
				 //"SMALL_REPAIR_USE_NUM,"
				 "REPAIR_START_TIME,"
				 "REPAIR_END_TIME,"
				 "MIDDLE_REPAIR_CASE,"
				 //"MIDDLE_REPAIR_NUM,"
				 "BIG_REPAIR_CASE,"
				 //"BIG_REPAIR_NUM,"
				 "REPAIR_GRP_NO,"
				 "REPAIR_SHIFT_NO,"
				 "REPAIR_WRITER,"
				 "REPAIR_REMARK,"
				 "REC_REVISOR,"
				 "REC_REVISE_TIME";
        ttmsm01.Update(sqlstr,"LADLE_NO");

		//大修需重置钢包包龄
		if(ttmsm01["MAINT_TYPE"].ToString().Trim() == "L")
		{
			ttmsm01["LADLE_LIFE"]     = 1;
			ttmsm01["LADLE_LEVEL"]    = "";
			ttmsm01["BIG_REPAIR_NUM"] = ttmsm01["BIG_REPAIR_NUM"].ToDecimal() + 1;
			ttmsm01.Update("LADLE_LIFE,BIG_REPAIR_NUM,LADLE_LEVEL","LADLE_NO");
		}

		//中修需重置钢包包龄
		if(ttmsm01["MAINT_TYPE"].ToString().Trim() == "M")
		{
			ttmsm01["LADLE_LIFE"]     = 1;
			ttmsm01["LADLE_LEVEL"]    = "";
			ttmsm01["MIDDLE_REPAIR_NUM"] = ttmsm01["MIDDLE_REPAIR_NUM"].ToDecimal() + 1;
			ttmsm01.Update("LADLE_LIFE,MIDDLE_REPAIR_NUM,LADLE_LEVEL","LADLE_NO");
		}

		//小修需重置钢包小修包龄
		if(ttmsm01["MAINT_TYPE"].ToString().Trim() == "S")
		{
			ttmsm01["SMALL_REPAIR_USE_NUM"] = 1;
			ttmsm01["SMALL_REPAIR_NUM"]     = ttmsm01["SMALL_REPAIR_NUM"].ToDecimal() + 1;
			ttmsm01.Update("SMALL_REPAIR_USE_NUM,SMALL_REPAIR_NUM","LADLE_NO");
		}

		////记录钢包履历
		//bcls_rec->Tables["TM0099"].Rows.Clear();
		//bcls_rec->Tables["TM0099"].Rows.Add();
		//bcls_rec->Tables["TM0099"].Rows[0]["LADLE_NO"]	 = ttmsm01["LADLE_NO"];
		//bcls_rec->Tables["TM0099"].Rows[0]["EVENT_ID"]	 = "TM09";
		//bcls_rec->Tables["TM0099"].Rows[0]["EVENT_DESC"] = "钢包维修";
		//bcls_rec->Tables["TM0099"].Rows[0]["FUNC_ID"]	 = "tmsm01a1f9_pro";
		//bcls_rec->Tables["TM0099"].Rows[0]["SYSTEM_ID"]  = "TMSM";
		//doFlag = f_tmsm_trace(bcls_rec, bcls_ret,conn);
		//if(doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}	

		//记录钢包履历
		//bcls_rec->Tables["TM0099"].Rows.Clear();
		ttmsm97.CopyFrom(ttmsm01);
		ttmsm97["EVENT_ID"] = "TM10";					// 事件号		
		ttmsm97["EVENT_DESC"] = "钢包使用";
		ttmsm97["FUNC_ID"] = s.svc_name;
		ttmsm97["MOLD_NO"] = ttmsm01["LADLE_NO"].ToString();			// 设备号
		ttmsm97["MOLD_TYPE"] = "2";			// 设备类型	TM40 1铁包，2钢包
		ttmsm97["STATUS"] = ttmsm01["LADLE_STATUS"];			// 钢包状态								
		ttmsm97.MergeTo(bcls_rec->Tables["TM0099"], false);

		doFlag = f_tmsm_trace97(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
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

