/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 结晶器信息查询
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 结晶器信息查询
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件

//外部函数声明

BM2F_ENTERACE(tmsm31bf2_inq) 

int f_tmsm31bf2_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;
	long  v_total_count = 0;
	long  fetchRowCount = 0;
	/* 业务变量 */
	CString	datetime("");	
	CString	cs_mold_no("");
	CString	cs_sm_unit_no("");	
	CString	cs_mold_type("");
	CDecimal cd_count	= 0;	
	CDecimal totalWt1 = 0;
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
	CDbCommand cmd_sql(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		CPageInfo pageInfo;
		try
		{
			//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			//2个变量信息是由前台的分页控件信息传入的，获取失败时,人工赋值一下。BY ZY ON 2011-11-30 19:10:51
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
			//EDLog(1,1,"pageInfo error[%s]",(const char*)ce.GetMsg());
		}
		//EDLog(1,1,"pageInfo.RecordFrom[%d]pageInfo.PageSize[%d]",pageInfo.RecordFrom,pageInfo.PageSize);


		/* 获取输入参数 */

		if (bcls_rec->Tables[0].Columns.Contains("MOLD_NO"))
			cs_mold_no = bcls_rec->Tables[0].Rows[0]["MOLD_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("SM_UNIT_NO"))
			cs_sm_unit_no = bcls_rec->Tables[0].Rows[0]["SM_UNIT_NO"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("MOLD_TYPE"))
			cs_mold_type = bcls_rec->Tables[0].Rows[0]["MOLD_TYPE"].ToString().Trim();
		
		Log::Info("", __FUNCTION__, "cs_mold_no		= [{0}]", cs_mold_no);
		Log::Info("", __FUNCTION__, "cs_sm_unit_no	= [{0}]", cs_sm_unit_no);
		Log::Trace("", __FUNCTION__,"cs_mold_type = [{0}]",  cs_mold_type);


		CString  c_sql_where = "  WHERE  1 = 1 "; //查询条件。
		CString c_sql_condition  = " SELECT * FROM ttmsm31  " ; 
		CString c_sql_condition2 = " SELECT COUNT(1) FROM ttmsm31  ";
		CString c_sql_orderBY = " ORDER BY MOLD_NO ASC ";

		if (cs_mold_no.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND MOLD_NO = @cs_mold_no " ;
		}
		if (cs_sm_unit_no.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND SM_UNIT_NO = @cs_sm_unit_no ";
		}
		if (cs_mold_type.Trim() != "")
		{
			c_sql_where = c_sql_where + " AND MOLD_TYPE = @cs_mold_type ";
		}
		
		//查询语句+ WHERE 语句。
		c_sql_condition = c_sql_condition + c_sql_where + c_sql_orderBY;
		c_sql_condition2 = c_sql_condition2 + c_sql_where;

		Log::Trace("", __FUNCTION__, "c_sql_condition[{0}]  ", c_sql_condition);
		Log::Trace("", __FUNCTION__, "c_sql_condition2[{0}]  ", c_sql_condition2);


		// 设置SQL中的变量
		cmd_sql.Parameters.Set("cs_mold_no", cs_mold_no);
		cmd_sql.Parameters.Set("cs_sm_unit_no", cs_sm_unit_no);
		cmd_sql.Parameters.Set("cs_mold_type", cs_mold_type);
		

		//获取汇总行数。(单记录获取模式)	
		//==============
		sqlstr = c_sql_condition2;
		cmd_sql.SetCommandText(c_sql_condition2);// 设置执行的SQL语句 

		//获取第一行+第一列的信息。
		v_total_count = cmd_sql.ExecuteScalar().ToInt32(); //获取第一行+第一列的信息。
		cmd_sql.Close(); //关闭游标  

		//获取查询返回的信息。(多记录获取模式)
		//============
		sqlstr = c_sql_condition;
		cmd_sql.SetCommandText(c_sql_condition);// 设置执行的SQL语句
		//逐行读取
		cmd_sql.ExecuteReader();
		cmd_sql.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		
		//返回的记录数。
		fetchRowCount = v_total_count;
		Log::Trace("", __FUNCTION__, "fetchRowCount[{0}]  ", fetchRowCount);

		/*设置系统返回参数*/
		{
			//_RES("GCRSS0000004")//查询到[{0}]条记录。
			CFormattable arguments[] = { fetchRowCount }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("GCRSS0000004"), arguments, 1); //格式化字符串 
		}

		/// <summary>
		/// 返回总记录数
		/// </summary>     
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = v_total_count;
	
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
		