/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     杨扬
Version:    1.0
Date:       2014-07-11
Description: 中间包信息烘烤
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 中间包信息烘烤
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
BM2F_ENTERACE(tmsm21bf8_pro)                                                     

int f_tmsm21bf8_pro(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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
	CModel ttmsm21("TTMSM21");
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
			ttmsm21.Reset();
			ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm21.TrimOrBlank();

			Log::Trace("",__FUNCTION__,"ttmsm21.SM_UNIT_NO			= [{0}]",(const char*)ttmsm21["SM_UNIT_NO"].ToString());
			Log::Trace("",__FUNCTION__,"ttmsm21.TD_NO		= [{0}]",(const char*)ttmsm21["TD_NO"].ToString());

			/* 检查输入参数合法性 */
			/*if(ttmsm21["SM_UNIT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"炼钢单元号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			if(ttmsm21["TD_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"中间包号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if(ttmsm21["DRYING_ST"].ToString().Trim() == "")
			{
				strcpy(s.msg,"烘烤开始时刻不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			/* 查询该钢包号是否存在 */
			if(ttmsm21.QueryCount("SM_UNIT_NO,TD_NO") <= 0)
			{
				sprintf(s.msg,"中间包号[%s]不存在!",(const char*)ttmsm21["TD_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			ttmsm21.Query("SM_UNIT_NO,TD_NO");
			ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			/* 查询钢包状态和流水号 */			 
			//switch(conn->DatabaseKind)
			//{
			//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//	case DB_KIND_MSSQL:				// MS SQL Server数据库
			//	case DB_KIND_ORACLE:	        // Oracle 数据库
			//	default:
			//		sqlstr = " SELECT TD_STATUS, "
			//			     "		  BAKE_SEQ_NO "
			//				 "   FROM ttmsm21 "
			//				 "  WHERE TD_NO 		= @ttmsm21.TD_NO ";		
			//		break;
			//}
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.Parameters.Set("ttmsm21.SM_UNIT_NO",ttmsm21["SM_UNIT_NO"].ToString());
			//cmd_inq.Parameters.Set("ttmsm21.TD_NO",ttmsm21["TD_NO"].ToString());	
			//cmd_inq.ExecuteReader();		
			//if(cmd_inq.Read())
			//{
			//	ttmsm21["TD_STATUS"] = cmd_inq.GetString(1);
			//	ttmsm21.BAKE_SEQ_NO  = cmd_inq.GetInt32(2);
			//}
			//cmd_inq.Close();

			/* 钢包状态不是烘烤,新增烘烤流水号 */
			//if(ttmsm21["TD_STATUS"].ToString().Trim() != "01")	//01-烘烤
			//{
			//	/* 生成烘烤流水号BAKE_SEQ_NO 需按项目组定制 */
			//	switch(conn->DatabaseKind)
			//	{
			//		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			//		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			//		case DB_KIND_MSSQL:				// MS SQL Server数据库
			//		case DB_KIND_ORACLE:	        // Oracle 数据库
			//		default:
			//			sqlstr = " SELECT NVL(MAX(BAKE_SEQ_NO),0) "
			//					 "   FROM ttmsm21 "
			//					 "  WHERE SM_UNIT_NO	= @ttmsm21.SM_UNIT_NO "
			//					 "    AND TD_NO	= @ttmsm21.TD_NO "		
			//					 "    AND LADLE_LIFE	= @ttmsm21.LADLE_LIFE ";		
			//			break;
			//	}
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.Parameters.Set("ttmsm21.SM_UNIT_NO",ttmsm21["SM_UNIT_NO"].ToString());
			//	cmd_inq.Parameters.Set("ttmsm21.TD_NO",ttmsm21["TD_NO"].ToString());	
			//	cmd_inq.Parameters.Set("ttmsm21.LADLE_LIFE",ttmsm21["LADLE_LIFE"].ToDecimal());	
			//	cmd_inq.ExecuteReader();		
			//	if(cmd_inq.Read())
			//	{
			//		ttmsm21.BAKE_SEQ_NO = cmd_inq.GetInt32(1) + 1;
			//	}
			//	cmd_inq.Close();
			//}
			//else
			//{
			//	/* 钢包状态是烘烤,烘烤流水号不变 */
			//	ttmsm21.BAKE_SEQ_NO	= ttmsm01.BAKE_SEQ_NO;
			//}
			

			/* 设置默认值 */
			ttmsm21["TD_STATUS"]	= "4";	//4-烘烤
			//f_epep_get_shift_group("BW",datetime,strShiftNo,strShiftGroup,conn);//取得班次班组
			//if(ttmsm21.BAKE_SHIFT_NO.Trim() == "")
			//{
			//	ttmsm21.BAKE_SHIFT_NO		= strShiftNo;		//生产班次号
			//}
			//if(ttmsm21.BAKE_GRP_NO.Trim()   == "")
			//{
			//	ttmsm21.BAKE_GRP_NO		    = strShiftGroup;		//生产班次号
			//}

			//if(ttmsm21.BAKE_WRITER.Trim()   == "")
			//{
			//	ttmsm21.BAKE_WRITER         = s.userid;
			//}

			//if(ttmsm21["DRYING_ST"].ToString().Trim() != "" && ttmsm21["DRYING_ET"].ToString().Trim() != "")
			//{
			//	CDateTime dt1 = CDateTime:: Parse(ttmsm21["DRYING_ST"].ToString());
			//	CDateTime dt2 = CDateTime:: Parse(ttmsm21["DRYING_ET"].ToString());
			//	CTimeSpan ts = dt2 - dt1;

			//	ttmsm21.ELAPSED_TIME	=ts.Days() * 24 * 60 + ts.Hours() * 60 + ts.Minutes();
			//	Log::Info("",__FUNCTION__,"持续时间 = [{0}]",ttmsm21.ELAPSED_TIME);

			//}

			/* 修改中间包信息 */			
			ttmsm21["REC_REVISOR"]		= s.userid;   //记录创建责任者
			ttmsm21["REC_REVISE_TIME"]	= datetime;   //记录创建时刻
			ttmsm21.TrimOrBlank();
			ttmsm21.Print();
			sqlstr = //"BAKE_SEQ_NO,"
				     "TD_STATUS,"
					 //"BAKE_TYPE,"
					 "BAKE_POS,"
					 //"BAKE_START_TEMP,"
					 //"BAKE_END_TEMP,"
					 "DRYING_ST,"
					 "DRYING_ET,"
					 //"ELAPSED_TIME,"
					 //"BAKE_SHIFT_NO,"
					 //"BAKE_GRP_NO,"
					 //"BAKE_WRITER,"
					 "DRYING_REMARK,"
					 "REC_REVISOR,"
					 "REC_REVISE_TIME";
            ttmsm21.Update(sqlstr,"SM_UNIT_NO,TD_NO");
			ttmsm97.CopyFrom(ttmsm21);
			ttmsm97["MOLD_TYPE"] = "3";
			ttmsm97["MOLD_NO"] = ttmsm21["TD_NO"];
			ttmsm97["EVENT_ID"] = "TM08";					// 事件号		
			ttmsm97["EVENT_DESC"] = "中间包烘烤";
			ttmsm97["FUNC_ID"] = s.svc_name;
			ttmsm97["STATUS"] = ttmsm21["TD_STATUS"];			// 产线类型								
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

