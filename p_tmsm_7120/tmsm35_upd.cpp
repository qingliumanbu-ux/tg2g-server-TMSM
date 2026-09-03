/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-22
功能: 结晶器维修实绩修改
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm35.h"
#include "ttmsm31.h"

/*<remark>=========================================================
/// <summary>
/// 结晶器维修实绩修改
/// <para>
/// 1.接收传入的结晶器维修实绩信息；
/// 2.修改结晶器维修实绩信息；
/// </para>
/// <para>数据库表：TTMSM35(结晶器维修实绩表)      </para>
/// <para>主调用函数：前台TTMSM21画面维修实绩修改调用；TTMSM22画面F4(修改)调用。   </para>
/// </summary>
/// <param name="bcls_rec">修改的结晶器维修实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm35_upd)

//void  f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn);

int f_tmsm35_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CTTMSM35 ttmsm35(conn);
	CTTMSM31 ttmsm31(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	
	try
	{			
		//记录总条数
		int rows = bcls_rec->Tables[0].Rows.get_Count();
		v_div = (CString)bcls_rec->Tables[1].Rows[0]["DIV"].ToString().Trim();
		Log::Trace("","tmsm35_upd","v_div [{0}]" ,(const char*)v_div);

		if(v_div.Trim() =="TMSM32")
		{
			//循环处理
			for (int i = 0; i < rows ; i++ )
			{
				// 取得单行传入信息 
				ttmsm35.Reset();
				ttmsm35.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				
				Log::Trace("","tmsm35_upd","ttmsm35.REPAIR_SEQ_NO [{0}]"	,ttmsm35.REPAIR_SEQ_NO);
				Log::Trace("","tmsm35_upd","ttmsm35.MOLD_NO [{0}]"			,(const char*)ttmsm35.MOLD_NO);
				Log::Trace("","tmsm35_upd","ttmsm35.MOLD_SECTION [{0}]"		,(const char*)ttmsm35.MOLD_SECTION);
				Log::Trace("","tmsm35_upd","ttmsm35.CC_MACH_NO [{0}]"		,(const char*)ttmsm35.CC_MACH_NO);
				Log::Trace("","tmsm35_upd","ttmsm35.SM_UNIT_NO [{0}]"		,(const char*)ttmsm35.SM_UNIT_NO);
				Log::Trace("","tmsm35_upd","ttmsm35.REPAIR_START_TIME [{0}]",(const char*)ttmsm35.REPAIR_START_TIME);
				Log::Trace("","tmsm35_upd","ttmsm35.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm35.REPAIR_END_TIME);
				Log::Trace("","tmsm35_upd","ttmsm35.MAINT_TYPE [{0}]"		,(const char*)ttmsm35.MAINT_TYPE);

				//班次班组 shift_no shift_group
				//f_epep_get_shift_group("DEFAULT",ttmsm35.REPAIR_START_TIME,v_shift,v_group,conn );

				// 判传入的参数是否有错	
				if(ttmsm35.REPAIR_SEQ_NO <= 0)
				{
					sprintf	(s.msg , "传入的维修流水号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				if(ttmsm35.SM_UNIT_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				if(ttmsm35.MOLD_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的结晶器号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				if(ttmsm35.MOLD_SECTION.Trim() == "")
				{
					sprintf	(s.msg , "传入的结晶器断面不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				if(ttmsm35.CC_MACH_NO.Trim() == "")
				{
					sprintf	(s.msg , "传入的结晶器所在连铸机号不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				if(ttmsm35.REPAIR_START_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修起始时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				if(ttmsm35.REPAIR_END_TIME.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修结束时间不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				if(ttmsm35.MAINT_TYPE.Trim() == "")
				{
					sprintf	(s.msg , "传入的维修类型不能为空") ;
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}

				
				/*查询结晶器维修实绩表*/
				sqlstr = " SELECT COUNT(1) "
						" FROM TTMSM35 "
						" WHERE REPAIR_SEQ_NO	= @ttmsm35.REPAIR_SEQ_NO "
						" AND SM_UNIT_NO		= @ttmsm35.SM_UNIT_NO "
						" AND MOLD_SECTION 		= @ttmsm35.MOLD_SECTION "
						" AND CC_MACH_NO		= @ttmsm35.CC_MACH_NO "
						" AND MOLD_NO			= @ttmsm35.MOLD_NO "						
						 ;
				cmd_inq.SetCommandText( sqlstr );
				cmd_inq.Parameters.Set( "ttmsm35.REPAIR_SEQ_NO"	, ttmsm35.REPAIR_SEQ_NO );
				cmd_inq.Parameters.Set( "ttmsm35.MOLD_NO"		, ttmsm35.MOLD_NO );
				cmd_inq.Parameters.Set( "ttmsm35.SM_UNIT_NO"	, ttmsm35.SM_UNIT_NO );
				cmd_inq.Parameters.Set( "ttmsm35.MOLD_SECTION"	, ttmsm35.MOLD_SECTION);
				cmd_inq.Parameters.Set( "ttmsm35.CC_MACH_NO"	, ttmsm35.CC_MACH_NO);
				cmd_inq.ExecuteReader();
				count = 0 ;
				if(cmd_inq.Read())
				{
					count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
				}
				Log::Trace("","tmsm35_upd","count {0}",count );
				cmd_inq.Close();
				if	(count == 0)
				{
					sprintf	(s.msg ,"结晶器维修实绩信息不存在");
					/*新增结晶器维修实绩信息报错信息档*/
					throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
				}
				
				//设置修改者信息
				ttmsm35.REC_REVISOR = s.userid;
				ttmsm35.REC_REVISE_TIME = datetimeNow;

				//根据传入信息更新结晶器维修实绩表
				sqlstr = "UPDATE TTMSM35"; //用于捕获数据库操作异常情况
				ttmsm35.Update(	" REC_REVISOR, "
								" REC_REVISE_TIME, "
								" MAINT_TYPE, "			//维修类型
								" REPAIR_POS, "			//维修位置
								" REPAIR_START_TIME, "	//维修开始时刻
								" REPAIR_END_TIME, "	//维修结束时刻
								" MAINTAIN_REMARK, "	//维修备注
								" REMARK ",				//备注
								" REPAIR_SEQ_NO,SM_UNIT_NO,MOLD_NO,MOLD_SECTION,CC_MACH_NO ");
			}
		}
		else if(v_div.Trim() == "TMSM31")
		{
			/*重置TTMSM35*/
			ttmsm31.Reset();
			/*读取传入的参数*/
			ttmsm31.MergeFrom(bcls_rec->Tables[0].Rows[0]);

			Log::Trace("","tmsm35_upd","ttmsm31.MOLD_NO [{0}]"			,(const char*)ttmsm31.MOLD_NO);
			Log::Trace("","tmsm35_upd","ttmsm31.MOLD_SECTION [{0}]"		,(const char*)ttmsm31.MOLD_SECTION);
			Log::Trace("","tmsm35_upd","ttmsm31.CC_MACH_NO [{0}]"		,(const char*)ttmsm31.CC_MACH_NO);
			Log::Trace("","tmsm35_upd","ttmsm31.SM_UNIT_NO [{0}]"		,(const char*)ttmsm31.SM_UNIT_NO);
			Log::Trace("","tmsm35_upd","ttmsm31.REPAIR_START_TIME [{0}]",(const char*)ttmsm31.REPAIR_START_TIME);
			Log::Trace("","tmsm35_upd","ttmsm31.REPAIR_END_TIME [{0}]"	,(const char*)ttmsm31.REPAIR_END_TIME);
			Log::Trace("","tmsm35_upd","ttmsm31.MAINT_TYPE [{0}]"		,(const char*)ttmsm31.MAINT_TYPE);

			// 判传入的参数是否有错			
			if(ttmsm31.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}
			if(ttmsm31.MOLD_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}
			if(ttmsm31.MOLD_SECTION.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器断面不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}
			if(ttmsm31.CC_MACH_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的结晶器所在连铸机号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}
			if(ttmsm31.REPAIR_START_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修起始时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}
			if(ttmsm31.REPAIR_END_TIME.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修结束时间不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}
			if(ttmsm31.MAINT_TYPE.Trim() == "")
			{
				sprintf	(s.msg , "传入的维修类型不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}

			/*查询结晶器维修实绩表*/
			sqlstr = "  SELECT COUNT(1) "
					 "  FROM TTMSM35 "
					 "  WHERE SM_UNIT_NO	= @ttmsm35.SM_UNIT_NO "					 
					 //"  AND MAINT_TYPE		= @ttmsm35.MAINT_TYPE "
					 " AND REPAIR_SEQ_NO	= @ttmsm35.REPAIR_SEQ_NO "
					 " AND MOLD_SECTION 	= @ttmsm35.MOLD_SECTION "
					 " AND CC_MACH_NO		= @ttmsm35.CC_MACH_NO "
					 " AND MOLD_NO			= @ttmsm35.MOLD_NO "

					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm35.SM_UNIT_NO"	, ttmsm31.SM_UNIT_NO );
			cmd_inq.Parameters.Set( "ttmsm35.REPAIR_SEQ_NO"	, ttmsm31.REPAIR_SEQ_NO );
			//cmd_inq.Parameters.Set( "ttmsm35.MAINT_TYPE"	, ttmsm31.MAINT_TYPE );
			cmd_inq.Parameters.Set( "ttmsm35.MOLD_NO"		, ttmsm31.MOLD_NO );
			cmd_inq.Parameters.Set( "ttmsm35.MOLD_SECTION"	, ttmsm31.MOLD_SECTION);
			cmd_inq.Parameters.Set( "ttmsm35.CC_MACH_NO"	, ttmsm31.CC_MACH_NO);
			cmd_inq.ExecuteReader();
			count = 0 ;
			if	(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中
				Log::Trace("","tmsm35_upd","count [{0}]",count );
			}
			cmd_inq.Close();
			if(count == 0)
			{
				sprintf	(s.msg ,"结晶器维修实绩信息不存在");
				/*新增结晶器维修实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm35_upd");
			}
			
			/*修改结晶器维修实绩表*/
			ttmsm35.CopyFrom(ttmsm31);
			//ttmsm35.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
			//ttmsm35.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */
			ttmsm35.REC_REVISOR = s.userid;
			ttmsm35.REC_REVISE_TIME = datetimeNow;

			ttmsm35.TrimOrBlank();
			sqlstr = "UPDATE TTMSM35"; //用于捕获数据库操作异常情况
			ttmsm35.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,MAINT_TYPE "				//维修类型
							" ,REPAIR_POS "
							" ,REPAIR_START_TIME "		//维修开始时刻
							" ,REPAIR_END_TIME "		//维修结束时刻
							" ,MAINTAIN_REMARK ",		//维修备注
							" REPAIR_SEQ_NO,SM_UNIT_NO,MOLD_NO,MOLD_SECTION,CC_MACH_NO ");

			sqlstr = "UPDATE TTMSM31";
			ttmsm31.Update(	" REC_REVISOR "
							" ,REC_REVISE_TIME "
							" ,REPAIR_SEQ_NO "
							" ,MAINT_TYPE "				//维修类型
							" ,REPAIR_POS "
							" ,REPAIR_START_TIME "		//维修开始时刻
							" ,REPAIR_END_TIME "		//维修结束时刻
							" ,MAINTAIN_REMARK ",		//维修备注
							" SM_UNIT_NO,MOLD_NO,MOLD_SECTION,CC_MACH_NO ");
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