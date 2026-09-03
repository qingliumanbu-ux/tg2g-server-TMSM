/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2013-05-24
Description: 结晶器信息使用
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 结晶器信息使用
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  

//外部函数声明
void f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn); //班次班别函数
int f_tmsm_trace97(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
BM2F_ENTERACE(tmsm31bf10_pro)                                                     

int f_tmsm31bf10_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CModel ttmsm31("TTMSM31");
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
		}
		bcls_rec->Tables["TM0099"].Rows.Clear();

		/* 获得传入参数 */
		for (int i = 0; i <  bcls_rec->Tables[0].Rows.get_Count() ; i++ )
		{
			ttmsm31.Reset();
			ttmsm31.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm31.TrimOrBlank();

			ttmsm31["SM_UNIT_NO"] = "A";

			Log::Trace("",__FUNCTION__,"ttmsm31.SM_UNIT_NO			= [{0}]",(const char*)ttmsm31["SM_UNIT_NO"].ToString());
			Log::Trace("",__FUNCTION__,"ttmsm31.MOLD_NO		= [{0}]",(const char*)ttmsm31["MOLD_NO"].ToString());

			/* 检查输入参数合法性 */
			/*if(ttmsm01.SM_UNIT_NO.Trim() == "")
			{
				strcpy(s.msg,"炼钢单元号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			if(ttmsm31["MOLD_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"结晶器号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			/*if(ttmsm31.USAGE_ST.Trim() == "")
			{
				strcpy(s.msg,"使用开始时刻不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/

			/* 查询该钢包号是否存在 */
			if(ttmsm31.QueryCount("SM_UNIT_NO,MOLD_NO") <= 0)
			{
				sprintf(s.msg,"结晶器号[%s]不存在!",(const char*)ttmsm31["MOLD_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			
			///* 查询钢包状态和流水号 */			 
			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = " SELECT MIT_STATUS, "
			//			     "		  USE_SEQ_NO "
			//				 "   FROM ttmsm31 "
			//				 "  WHERE SM_UNIT_NO	= @ttmsm31.SM_UNIT_NO "
			//				 "    AND MOLD_NO	= @ttmsm31.MOLD_NO "
			//				 "    AND OP_DIV	    = @ttmsm31.OP_DIV ";		
			//		break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("ttmsm31.SM_UNIT_NO",ttmsm31["SM_UNIT_NO"].ToString());
			//cmd_inq.Parameters.Set("ttmsm31.LADLE_NO",ttmsm31.LADLE_NO);
			//cmd_inq.Parameters.Set("ttmsm31.OP_DIV",ttmsm31.OP_DIV);	
			//cmd_inq.ExecuteReader();		
			//if(cmd_inq.Read())
			//{
			//	ttmsm31.MIT_STATUS = cmd_inq.GetString(1);
			//	ttmsm31.USE_SEQ_NO   = cmd_inq.GetInt32(2);
			//}
			//cmd_inq.Close();

			///* 钢包状态不是使用,新增使用流水号 */
			//if(ttmsm31.MIT_STATUS.Trim() != "00")	//21-使用
			//{
			//	/* 生成使用流水号USE_SEQ_NO 需按项目组定制 */
			//	switch(conn->DatabaseKind)
			//	{
			//		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//		case DB_KIND_MSSQL:				// MS SQL Server数据库
			//		case DB_KIND_ORACLE:	        // Oracle 数据库
			//		default:
			//			sqlstr = " SELECT NVL(MAX(USE_SEQ_NO),0) "
			//					 "   FROM ttmsm31 "
			//					 "  WHERE SM_UNIT_NO	= @ttmsm31.SM_UNIT_NO "
			//					 "    AND LADLE_NO 		= @ttmsm31.LADLE_NO "		
			//					 "    AND LADLE_LIFE	= @ttmsm31.LADLE_LIFE "
			//					 "    AND OP_DIV	    = @ttmsm31.OP_DIV ";		
			//			break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("ttmsm31.SM_UNIT_NO",ttmsm31["SM_UNIT_NO"].ToString());
			//	cmd_inq.Parameters.Set("ttmsm31.LADLE_NO",ttmsm31.LADLE_NO);	
			//	cmd_inq.Parameters.Set("ttmsm31.LADLE_LIFE",ttmsm31.LADLE_LIFE);	
			//	cmd_inq.Parameters.Set("ttmsm31.OP_DIV",ttmsm31.OP_DIV);
			//	cmd_inq.ExecuteReader();		
			//	if(cmd_inq.Read())
			//	{
			//		ttmsm31["USE_SEQ_NO"] = cmd_inq.GetInt32(1) + 1;
			//	}
			//	cmd_inq.Close();
			//}
			//else
			//{
			//	/* 钢包状态是使用,使用流水号不变 */
			//	ttmsm31.USE_SEQ_NO	= ttmsm31["USE_SEQ_NO"];
			//}

			
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:
						sqlstr = " SELECT CURRENT_STATUS "
								 "   FROM ttmsm31 "
								 "  WHERE SM_UNIT_NO	= @ttmsm31.SM_UNIT_NO "
								 "    AND MOLD_NO 		= @ttmsm31.MOLD_NO ";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("ttmsm31.SM_UNIT_NO", ttmsm31["SM_UNIT_NO"].ToString());
				cmd_inq.Parameters.Set("ttmsm31.MOLD_NO", ttmsm31["MOLD_NO"].ToString());
				cmd_inq.ExecuteReader();		
				if(cmd_inq.Read())
				{
					ttmsm31["CURRENT_STATUS"] = cmd_inq.GetString(1);
				}
				cmd_inq.Close();
			/* 设置默认值 */
			if (ttmsm31["CURRENT_STATUS"].ToString().Trim() == "3")
			{
				sprintf(s.msg, "结晶器[%s]已报废，不能使用!", (const char*)ttmsm31["MOLD_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else if (ttmsm31["CURRENT_STATUS"].ToString().Trim() == "0" )
			{
				sprintf(s.msg, "结晶器[%s]已上线!", (const char*)ttmsm31["MOLD_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			else
			{
				ttmsm31["CURRENT_STATUS"] = "0";
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT NVL(MAX(USE_SEQ_NO),0) "
						"   FROM ttmsm31 "
						"  WHERE SM_UNIT_NO	= @ttmsm31.SM_UNIT_NO "
						"    AND MOLD_NO 		= @ttmsm31.MOLD_NO ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("ttmsm31.SM_UNIT_NO", ttmsm31["SM_UNIT_NO"].ToString());
				cmd_inq.Parameters.Set("ttmsm31.MOLD_NO", ttmsm31["MOLD_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					ttmsm31["USE_SEQ_NO"] = cmd_inq.GetInt32(1) + 1;
				}
				cmd_inq.Close();
			}
			
			//f_epep_get_shift_group("BW",datetime,strShiftNo,strShiftGroup,conn);//取得班次班组
			//ttmsm31.SHIFT_NO		= strShiftNo;		//生产班次号
			//ttmsm31.GRP_NO			= strShiftGroup;	//生产班组

			/* 修改钢包信息 */		
			ttmsm31["REC_REVISOR"]		= s.userid;   //记录创建责任者
			ttmsm31["REC_REVISE_TIME"]	= datetime;   //记录创建时刻
			ttmsm31.TrimOrBlank();
			ttmsm31.Print();

			/* 修改钢包信息 */
			ttmsm31["REC_REVISOR"] = s.userid;   //记录创建责任者
			ttmsm31["REC_REVISE_TIME"] = datetime;   //记录创建时刻
			sqlstr = " REC_REVISOR    "
					 ",REC_REVISE_TIME "
					 ", CURRENT_STATUS ";
			for (i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
			{
				CString sColName = bcls_rec->Tables[0].Columns[i].get_ColumnName().ToUpper();

				if (ttmsm31.GetFields().Contains(sColName) &&
					sColName != "SM_UNIT_NO" &&
					sColName != "MOLD_NO" &&
					sColName != "MOLD_TYPE" &&
					sColName != "REC_REVISOR" &&
					sColName != "REC_REVISE_TIME")
				{
					sqlstr = sqlstr + "," + sColName;
				}
			}
			Log::Trace("", __FUNCTION__, "sqlstr	= [{0}]", (const char*)sqlstr);

			ttmsm31.Update(sqlstr, "SM_UNIT_NO,MOLD_NO");
			//sqlstr = " SM_UNIT_NO    "
			//		 ",MOLD_NO "
			//		 ",SHIFT_NO      "
			//		 ",GRP_NO        "
			//		 ",MIT_STATUS    " 
			//		 ",LADLE_LIFE    "
			//		 ",SHELL_FAY     "
			//		 ",WORK_MAKER    "
			//		 ",REPAIR_POS    "
			//		 ",EMPTY_LADLE_WT"
			//		 ",MOLTIRON_WT3  "
			//		 ",MOLTIRON_WT4  "
			//		 ",REMARK        "
			//		 ",REC_REVISOR      "
			//		 ",REC_REVISE_TIME  "
					//;
            ttmsm31.Update(sqlstr,"SM_UNIT_NO,MOLD_NO");
			ttmsm31.Query("SM_UNIT_NO,MOLD_NO");
			//记录钢包履历
			//bcls_rec->Tables["TM0099"].Rows.Clear();
			ttmsm97.CopyFrom(ttmsm31);
			ttmsm97["EVENT_ID"] = "TM10";					// 事件号		
			ttmsm97["EVENT_DESC"] = "使用";
			ttmsm97["FUNC_ID"] = s.svc_name;
			ttmsm97["STATUS"] = ttmsm31["CURRENT_STATUS"];			// 产线类型								
			ttmsm97.MergeTo(bcls_rec->Tables["TM0099"], false);
		}
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
