
//GlobusXIODeclareModule(file); 
GlobusXIODeclareModule(gsi); 
//GlobusXIODeclareModule(http); 
//GlobusXIODeclareModule(mode_e); 
//GlobusXIODeclareModule(ordering); 
//GlobusXIODeclareModule(queue); 
GlobusXIODeclareModule(tcp); 
//GlobusXIODeclareModule(telnet); 
//GlobusXIODeclareModule(udp); 
//GlobusXIODeclareModule(udt); 

static globus_extension_builtin_t       local_extensions[] = 
{
//  {GlobusXIOExtensionName(file), GlobusXIOMyModule(file)}, 
  {GlobusXIOExtensionName(gsi), GlobusXIOMyModule(gsi)},
//  {GlobusXIOExtensionName(http), GlobusXIOMyModule(http)},
//  {GlobusXIOExtensionName(mode_e), GlobusXIOMyModule(mode_e)},
//  {GlobusXIOExtensionName(ordering), GlobusXIOMyModule(ordering)},
//  {GlobusXIOExtensionName(queue), GlobusXIOMyModule(queue)},
  {GlobusXIOExtensionName(tcp), GlobusXIOMyModule(tcp)},
//  {GlobusXIOExtensionName(telnet), GlobusXIOMyModule(telnet)},
//  {GlobusXIOExtensionName(udp), GlobusXIOMyModule(udp)},
//  {GlobusXIOExtensionName(udt), GlobusXIOMyModule(udt)}, 
  {NULL, NULL}
};
