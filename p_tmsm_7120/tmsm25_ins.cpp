/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-22
功能: 中间罐维修实绩新增
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm25.h"
#include "ttmsm21.h"

/*<remark>=========================================================
/// <summary>
/// 中间罐维修实绩新增
/// <para>
/// 1.接收传入的中间罐维修实绩信息；
/// 2.新增中间罐维修实绩信息；
/// </para>
/// <para>数据库表：TTMSM25(中间罐维修实绩表)      </para>
/// <para>主调用函数：前台TTMSM21画面维修实绩新增调用；TTMSM22画面F3(新增)调用。   </para>
/// </summary>
/// <param name="bcls_rec">新增的中间罐维修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm25_ins)

//void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm25_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */
	CString v_group = "";
	CString v_shift = "";
	CString v_div = "";
	
	/* 实体类定义 */
	CTTMSM25 ttmsm25(conn);
	CTTMSM21 ttmsm21(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{	
		//记录总条数
		int	rows = bcls_rec->Tables[0].Rows.get_Count(); 
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm25_ins","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM22")
		{
			for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
			{
				/*重置TTMSM25*/
				ttmsm25.Reset();
				/*读取传入的参数*/
				ttmsm25.MergeFrom(bcls_rec->Tables[0].Rows[fetchRowCount]);

				Log::Trace("","tmsm25_ins","ttmsm25.TD_NO [{0}]"			,(const char*)ttmsm25.TD_NO);
				Log::Trace("","tmsm25_ins","ttmsm25.SM_UNIT_NO [{0}]"		,(const char*)ttmsm25.SM_UNIT_NO);
				Log::Trace("","tmsm25_ins","ttmsm25.REPAIR_START_TIME [{0}]",(const char*)ttmsm25.REPAIR_START_TIME);
				Log::Trace("","tmsm25_ins","ttmsm25.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm25.REPAIR_END_TIME);
				Log::Trace("","tmsm25_ins","ttmsm25.MAINT_TYPE [{0}]"		,(const char*)ttmsm25.MAINT_TYPE);

				//班次班组 shift_no shift_group
				//f_epep_get_shift_group("DEFAULT",ttmsm25.REPAIR_START_TIME,v_shift,v_group,conn );

				// 判传入的参数是否有错			
				if(ttmsm25.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
				}
				if(ttmsm25.TD_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的中间罐号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
				}
				if(ttmsm25.REPAIR_START_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
				}
				if(ttmsm25.REPAIR_END_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修结束时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
				}
				if(ttmsm25.MAINT_TYPE.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修类型不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
				}

				/*查询中间罐维修实绩表*/
				sqlstr = "  SELECT COUNT(1) "
						 "  FROM TTMSM25 "
						 "  WHERE SM_UNIT_NO		= @ttmsm25.SM_UNIT_NO "
						 "  AND REPAIR_START_TIME	= @ttmsm25.REPAIR_START_TIME "
						 "  AND MAINT_TYPE			= @ttmsm25.MAINT_TYPE "
						 "  AND TD_NO				= @ttmsm25.TD_NO "
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm25.SM_UNIT_NO"		, ttmsm25.SM_UNIT_NO );
				cmd_inq.Parameters.Set( "ttmsm25.REPAIR_START_TIME"	, ttmsm25.REPAIR_START_TIME );
				cmd_inq.Parameters.Set( "ttmsm25.MAINT_TYPE"		, ttmsm25.MAINT_TYPE );
				cmd_inq.Parameters.Set( "ttmsm25.TD_NO"				, ttmsm25.TD_NO );
				cmd_inq.ExecuteReader();
				count = 0 ;
				if	(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
					Log::Trace("","tmsm25_ins","count [{0}]",count );
				}
				cmd_inq.Close();
				if(count >= 1)
				{
					sprintf	(s.msg ,"中间罐维修实绩信息已存在");
					/*新增中间罐维修实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
				}
				
				// 生成维修流水号BAKE_SEQ_NO
				sqlstr =	" SELECT nvl(MAX(REPAIR_SEQ_NO),0) "
							" FROM TTMSM25 "
							" WHERE SM_UNIT_NO	= @ttmsm25.SM_UNIT_NO "
							" AND TD_NO 		= @ttmsm25.TD_NO "
							;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set("ttmsm25.SM_UNIT_NO"	, ttmsm25.SM_UNIT_NO);
				cmd_inq.Parameters.Set("ttmsm25.TD_NO"		, ttmsm25.TD_NO);
				cmd_inq.ExecuteReader();
				if ( cmd_inq.Read() )
				{
					ttmsm25.REPAIR_SEQ_NO = cmd_inq.GetInt32(1)+1;
				}
				cmd_inq.Close();
				Log::Trace("","tmsm25_ins","ttmsm25.REPAIR_SEQ_NO [{0}]",ttmsm25.REPAIR_SEQ_NO);

				/*新增中间罐维修实绩表*/
				ttmsm25.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
				ttmsm25.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */ 
				//s.datetime是前台终端操作系统的时间，而不是服务器时间
				//if(ttmsm25.GRP_NO.Trim() == "")
				//{
				//	ttmsm25.GRP_NO = v_group;
				//}
				//if(ttmsm25.SHIFT_NO.Trim() == "")
				//{
				//	ttmsm25.SHIFT_NO = v_shift;
				//}

				ttmsm25.TrimOrBlank();
				ttmsm25.Insert();
			}
		}
		else if(v_div.Trim() == "TMSM21")
		{
			/*重置TTMSM21*/
			ttmsm21.Reset();
			/*读取传入的参数*/
			ttmsm21.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm25_ins","ttmsm21.TD_NO [{0}]"			,(const char*)ttmsm21.TD_NO);
			Log::Trace("","tmsm25_ins","ttmsm21.SM_UNIT_NO [{0}]"		,(const char*)ttmsm21.SM_UNIT_NO);
			Log::Trace("","tmsm25_ins","ttmsm21.REPAIR_START_TIME [{0}]",(const char*)ttmsm21.REPAIR_START_TIME);
			Log::Trace("","tmsm25_ins","ttmsm21.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm21.REPAIR_END_TIME);
			Log::Trace("","tmsm25_ins","ttmsm21.MAINT_TYPE [{0}]"		,(const char*)ttmsm21.MAINT_TYPE);

			// 判传入的参数是否有错			
			if(ttmsm21.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
			}
			if(ttmsm21.TD_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的中间罐号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
			}
			if(ttmsm21.REPAIR_START_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
			}
			//if(ttmsm21.REPAIR_END_TIME.Trim() == "")
			//{
			//	sprintf	(s.msg , "传入的维修结束时间不能为空") ;
			//	throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
			//}
			if(ttmsm21.MAINT_TYPE.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修类型不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
			}

			/*查询中间罐维修实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM25 "
					 "  WHERE SM_UNIT_NO		= @ttmsm25.SM_UNIT_NO "
					 "  AND REPAIR_START_TIME	= @ttmsm25.REPAIR_START_TIME "
					 "  AND MAINT_TYPE			= @ttmsm25.MAINT_TYPE "
					 "  AND TD_NO				= @ttmsm25.TD_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm25.SM_UNIT_NO"		, ttmsm21.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm25.REPAIR_START_TIME"	, ttmsm21.REPAIR_START_TIME );
			cmd_inq.Parameters.Set( "ttmsm25.MAINT_TYPE"		, ttmsm21.MAINT_TYPE );
			cmd_inq.Parameters.Set( "ttmsm25.TD_NO"				, ttmsm21.TD_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm25_ins","count {0}",count );
			}
			cmd_inq.Close();
			if(count >= 1)
			{
				sprintf	(s.msg ,"中间罐维修实绩信息已存在");
				/*新增中间罐维修实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm25_ins");
			}
			
			// 生成维修流水号REPAIR_SEQ_NO
			sqlstr =	" SELECT nvl(MAX(REPAIR_SEQ_NO),0) "
						" FROM TTMSM25 "
						" WHERE SM_UNIT_NO	= @ttmsm25.SM_UNIT_NO "
						" AND TD_NO 		= @ttmsm25.TD_NO "
						;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set("ttmsm25.SM_UNIT_NO"	, ttmsm21.SM_UNIT_NO);
			cmd_inq.Parameters.Set("ttmsm25.TD_NO"		, ttmsm21.TD_NO);
			cmd_inq.ExecuteReader();
			if ( cmd_inq.Read() )
			{
				ttmsm21.REPAIR_SEQ_NO = cmd_inq.GetInt32(1)+1;
			}
			cmd_inq.Close();
			Log::Trace("","tmsm25_ins","ttmsm21.REPAIR_SEQ_NO [{0}]",ttmsm21.REPAIR_SEQ_NO);

			/*新增中间罐维修实绩表*/
			ttmsm25.CopyFrom(ttmsm21);
			ttmsm25.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			ttmsm25.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm25.REC_REVISOR = " ";
			ttmsm25.REC_REVISE_TIME = " ";
			Log::Trace("","tmsm25_ins","ttmsm25.MAINT_TYPE [{0}]",(const char*)ttmsm25.MAINT_TYPE);
			ttmsm25.TrimOrBlank();
			ttmsm25.Insert();

			sqlstr = "UPDATE TTMSM21";
			ttmsm21.TD_STATUS = "21"; // 01-可用 11-使用 21-维修 99-报废
			ttmsm21.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,TD_STATUS "
							" ,REPAIR_SEQ_NO "
							" ,MAINT_TYPE "				//维修类型
							" ,REPAIR_POS "
							" ,REPAIR_START_TIME "		//维修开始时刻
							" ,REPAIR_END_TIME "		//维修结束时刻
							" ,MAINTAIN_REMARK ",		//维修备注
							" SM_UNIT_NO,TD_NO ");
		}
		//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);	
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		//CMessageFormat::Format(s.msg,  _RES("读取数据失败,表[{0}],sqlcode=[{1}]。请联系系统维护人员."), arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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