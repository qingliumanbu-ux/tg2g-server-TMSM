/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-01
Description: 钢包信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 钢包信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(tmsm01a1f2_inq) 

int f_tmsm01a1f2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");	
	CString	cs_ladle_no("");	
	CString	cs_sm_unit_no("");	
	CDecimal cd_count	= 0;								
	int	record_count_per_page	= 0; /* 每页记录数 */
	int	current_page_no			= 0; /* 需查询的页号,从0开始计数 */
	int	start_row				= 0; /* 将要压入outBlock的起始行 */
    
	/* 实体类定义 */ 

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_count;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	 
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获取输入参数 */
		cs_ladle_no				= bcls_rec->Tables[0].Rows[0]["LADLE_NO"].ToString().Trim();    //钢包号
		cs_sm_unit_no			= bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"].ToString().Trim();	//炼钢单元号
		record_count_per_page	= bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];	
		current_page_no			= bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];	
		
		Log::Trace("",__FUNCTION__,"cs_ladle_no				= [{0}]",(const char*)cs_ladle_no);
		Log::Trace("",__FUNCTION__,"cs_sm_unit_no			= [{0}]",(const char*)cs_sm_unit_no);
		Log::Trace("",__FUNCTION__,"record_count_per_page	= [{0}]",record_count_per_page);
		Log::Trace("",__FUNCTION__,"current_page_no			= [{0}]",current_page_no);


		/* 查询钢包信息 */   
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr_count = " SELECT COUNT(1) "
							   "   FROM TTMSM01 "
							   "  WHERE 1 = 1 ";
				sqlstr	= " SELECT * "
						  "   FROM TTMSM01 "
						  "  WHERE 1 = 1 ";
				if(cs_ladle_no.Trim() != "")
				{
					sqlstr_temp	+= " AND LADLE_NO LIKE @cs_ladle_no ||'%' "; 
				}	
				if(cs_sm_unit_no.Trim() != "")
				{
					sqlstr_temp	+= " AND SM_UNIT_NO LIKE @cs_sm_unit_no ||'%' "; 
				}												
				
				sqlstr_count = sqlstr_count + sqlstr_temp;
				sqlstr_temp += " ORDER BY LADLE_NO ASC";
				sqlstr		 = sqlstr + sqlstr_temp;
				break;
		}     
		Log::Trace("",__FUNCTION__,"sqlstr_temp			= [{0}]",(const char*)sqlstr_temp);
		Log::Trace("",__FUNCTION__,"sqlstr_count		= [{0}]",(const char*)sqlstr_count);
		Log::Trace("",__FUNCTION__,"sqlstr				= [{0}]",(const char*)sqlstr);
		cmd_inq.Parameters.Clear();
		if(cs_ladle_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_ladle_no",cs_ladle_no);
		}
		if(cs_sm_unit_no.Trim() != "")
		{
			cmd_inq.Parameters.Set("cs_sm_unit_no",cs_sm_unit_no); 
		}
		cmd_inq.SetCommandText(sqlstr_count);
		cd_count = cmd_inq.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if(start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}
		cmd_inq.Close();
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0],start_row,record_count_per_page);
		cmd_inq.Close();

		//返回分页信息 
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0]	= cd_count.ToInt32();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
		