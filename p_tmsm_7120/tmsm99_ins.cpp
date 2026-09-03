/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2014-01-20
功能: TMSM99 铁包包龄计算公式新增
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
//#include "ttmsm99.h"

/*<remark>=========================================================
/// <summary>
/// 铁包维修实绩新增
/// <para>
/// 1.接收传入的铁包包龄计算公式信息；
/// 2.新增铁包包龄计算公式信息；
/// </para>
/// <para>数据库表：TTMSM99(铁包包龄计算公式表)      </para>
/// <para>主调用函数：前台TTMSM11画面新增调用。   </para>
/// </summary>
/// <param name="bcls_rec">新增的铁包铁包包龄计算公式信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm99_ins)

int f_tmsm99_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */
	//CString v_group = "";
	
	/* 实体类定义 */
	//CTTMSM99 ttmsm99(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{	
		////记录总条数
		//int	rows = bcls_rec->Tables[0].Rows.get_Count(); 
		//for(int fetchRowCount = 0; fetchRowCount < rows; fetchRowCount++)
		//{
		//	/*重置TTMSM15*/
		//	ttmsm99.Reset();
		//	/*读取传入的参数*/
		//	ttmsm99.MergeFrom(bcls_rec->Tables[0].Rows[fetchRowCount]);
		//	ttmsm99.Print();
		//	
		//	// 判传入的参数是否有错			
		//	if(ttmsm99.MODEL_ID.Trim() == "")
		//	{
		//		sprintf	(s.msg , "传入的公式号不能为空") ;
		//		throw	CApplicationException(-1 , s.msg , "tmsm99_ins");
		//	}

		//	/*新增铁包维修实绩表*/
		//	ttmsm99.REC_CREATOR		 =	s.userid;		/* 记录创建责任者 */
		//	ttmsm99.REC_CREATE_TIME	 =	datetimeNow; 	/* 记录创建时刻 */ 
		//	ttmsm99.TrimOrBlank();
		//	ttmsm99.Insert();
		//}
		
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