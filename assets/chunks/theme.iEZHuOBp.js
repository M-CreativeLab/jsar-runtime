const __vite__mapDeps=(i,m=__vite__mapDeps,d=(m.f||(m.f=["assets/chunks/VPLocalSearchBox.-wtWpN5J.js","assets/chunks/framework.DYgWcJ6Z.js"])))=>i.map(i=>d[i]);
var _u=Object.defineProperty;var vu=(i,e,t)=>e in i?_u(i,e,{enumerable:!0,configurable:!0,writable:!0,value:t}):i[e]=t;var _t=(i,e,t)=>vu(i,typeof e!="symbol"?e+"":e,t);import{d as Ce,c as F,r as J,n as ct,o as L,a as Xn,t as xe,b as De,w as te,T as co,e as _e,_ as be,u as xu,i as Mu,f as Su,g as Oi,h as qe,j as w,k as V,l as ci,m as da,p as We,q as Un,s as hr,v as zt,x as jn,y as uo,z as yu,A as bu,F as tt,B as vt,C as fi,D as cc,E as fr,G as Ie,H as vn,I as uc,J as pr,K as Gn,L as mr,M as ha,N as Eu,O as ho,P as fa,Q as dc,R as hc,S as gr,U as Tu,V as wu,W as Au,X as fc,Y as pc,Z as Ru,$ as Cu,a0 as Pu,a1 as Lu,a2 as Ui,a3 as fo,a4 as pa,a5 as mc}from"./framework.DYgWcJ6Z.js";const Du=Ce({__name:"VPBadge",props:{text:{},type:{default:"tip"}},setup(i){return(e,t)=>(L(),F("span",{class:ct(["VPBadge",e.type])},[J(e.$slots,"default",{},()=>[Xn(xe(e.text),1)])],2))}}),Iu={key:0,class:"VPBackdrop"},Uu=Ce({__name:"VPBackdrop",props:{show:{type:Boolean}},setup(i){return(e,t)=>(L(),De(co,{name:"fade"},{default:te(()=>[e.show?(L(),F("div",Iu)):_e("",!0)]),_:1}))}}),ku=be(Uu,[["__scopeId","data-v-c79a1216"]]),Qe=xu;function Nu(i,e){let t,n=!1;return()=>{t&&clearTimeout(t),n?t=setTimeout(i,e):(i(),(n=!0)&&setTimeout(()=>n=!1,e))}}function ma(i){return i.startsWith("/")?i:`/${i}`}function po(i){const{pathname:e,search:t,hash:n,protocol:s}=new URL(i,"http://a.com");if(Mu(i)||i.startsWith("#")||!s.startsWith("http")||!Su(e))return i;const{site:r}=Qe(),a=e.endsWith("/")||e.endsWith(".html")?i:i.replace(/(?:(^\.+)\/)?.*$/,`$1${e.replace(/(\.md)?$/,r.value.cleanUrls?"":".html")}${t}${n}`);return Oi(a)}function fs({correspondingLink:i=!1}={}){const{site:e,localeIndex:t,page:n,theme:s,hash:r}=Qe(),a=qe(()=>{var l,c;return{label:(l=e.value.locales[t.value])==null?void 0:l.label,link:((c=e.value.locales[t.value])==null?void 0:c.link)||(t.value==="root"?"/":`/${t.value}/`)}});return{localeLinks:qe(()=>Object.entries(e.value.locales).flatMap(([l,c])=>a.value.label===c.label?[]:{text:c.label,link:Fu(c.link||(l==="root"?"/":`/${l}/`),s.value.i18nRouting!==!1&&i,n.value.relativePath.slice(a.value.link.length-1),!e.value.cleanUrls)+r.value})),currentLang:a}}function Fu(i,e,t,n){return e?i.replace(/\/$/,"")+ma(t.replace(/(^|\/)index\.md$/,"$1").replace(/\.md$/,n?".html":"")):i}const Ou={class:"NotFound"},Bu={class:"code"},zu={class:"title"},Vu={class:"quote"},Hu={class:"action"},Gu=["href","aria-label"],Wu=Ce({__name:"NotFound",setup(i){const{theme:e}=Qe(),{currentLang:t}=fs();return(n,s)=>{var r,a,o,l,c;return L(),F("div",Ou,[w("p",Bu,xe(((r=V(e).notFound)==null?void 0:r.code)??"404"),1),w("h1",zu,xe(((a=V(e).notFound)==null?void 0:a.title)??"PAGE NOT FOUND"),1),s[0]||(s[0]=w("div",{class:"divider"},null,-1)),w("blockquote",Vu,xe(((o=V(e).notFound)==null?void 0:o.quote)??"But if you don't change your direction, and if you keep looking, you may end up where you are heading."),1),w("div",Hu,[w("a",{class:"link",href:V(Oi)(V(t).link),"aria-label":((l=V(e).notFound)==null?void 0:l.linkLabel)??"go to home"},xe(((c=V(e).notFound)==null?void 0:c.linkText)??"Take me home"),9,Gu)])])}}}),$u=be(Wu,[["__scopeId","data-v-d6be1790"]]);function gc(i,e){if(Array.isArray(i))return Ys(i);if(i==null)return[];e=ma(e);const t=Object.keys(i).sort((s,r)=>r.split("/").length-s.split("/").length).find(s=>e.startsWith(ma(s))),n=t?i[t]:[];return Array.isArray(n)?Ys(n):Ys(n.items,n.base)}function Xu(i){const e=[];let t=0;for(const n in i){const s=i[n];if(s.items){t=e.push(s);continue}e[t]||e.push({items:[]}),e[t].items.push(s)}return e}function qu(i){const e=[];function t(n){for(const s of n)s.text&&s.link&&e.push({text:s.text,link:s.link,docFooterText:s.docFooterText}),s.items&&t(s.items)}return t(i),e}function ga(i,e){return Array.isArray(e)?e.some(t=>ga(i,t)):ci(i,e.link)?!0:e.items?ga(i,e.items):!1}function Ys(i,e){return[...i].map(t=>{const n={...t},s=n.base||e;return s&&n.link&&(n.link=s+n.link),n.items&&(n.items=Ys(n.items,s)),n})}function kn(){const{frontmatter:i,page:e,theme:t}=Qe(),n=da("(min-width: 960px)"),s=We(!1),r=qe(()=>{const _=t.value.sidebar,m=e.value.relativePath;return _?gc(_,m):[]}),a=We(r.value);Un(r,(_,m)=>{JSON.stringify(_)!==JSON.stringify(m)&&(a.value=r.value)});const o=qe(()=>i.value.sidebar!==!1&&a.value.length>0&&i.value.layout!=="home"),l=qe(()=>c?i.value.aside==null?t.value.aside==="left":i.value.aside==="left":!1),c=qe(()=>i.value.layout==="home"?!1:i.value.aside!=null?!!i.value.aside:t.value.aside!==!1),u=qe(()=>o.value&&n.value),d=qe(()=>o.value?Xu(a.value):[]);function h(){s.value=!0}function p(){s.value=!1}function g(){s.value?p():h()}return{isOpen:s,sidebar:a,sidebarGroups:d,hasSidebar:o,hasAside:c,leftAside:l,isSidebarEnabled:u,open:h,close:p,toggle:g}}function Yu(i,e){let t;hr(()=>{t=i.value?document.activeElement:void 0}),zt(()=>{window.addEventListener("keyup",n)}),jn(()=>{window.removeEventListener("keyup",n)});function n(s){s.key==="Escape"&&i.value&&(e(),t==null||t.focus())}}function ju(i){const{page:e,hash:t}=Qe(),n=We(!1),s=qe(()=>i.value.collapsed!=null),r=qe(()=>!!i.value.link),a=We(!1),o=()=>{a.value=ci(e.value.relativePath,i.value.link)};Un([e,i,t],o),zt(o);const l=qe(()=>a.value?!0:i.value.items?ga(e.value.relativePath,i.value.items):!1),c=qe(()=>!!(i.value.items&&i.value.items.length));hr(()=>{n.value=!!(s.value&&i.value.collapsed)}),uo(()=>{(a.value||l.value)&&(n.value=!1)});function u(){s.value&&(n.value=!n.value)}return{collapsed:n,collapsible:s,isLink:r,isActiveLink:a,hasActiveLink:l,hasChildren:c,toggle:u}}function Zu(){const{hasSidebar:i}=kn(),e=da("(min-width: 960px)"),t=da("(min-width: 1280px)");return{isAsideEnabled:qe(()=>!t.value&&!e.value?!1:i.value?t.value:e.value)}}const Ku=/\b(?:VPBadge|header-anchor|footnote-ref|ignore-header)\b/,_a=[];function _c(i){return typeof i.outline=="object"&&!Array.isArray(i.outline)&&i.outline.label||i.outlineTitle||"On this page"}function mo(i){const e=[...document.querySelectorAll(".VPDoc :where(h1,h2,h3,h4,h5,h6)")].filter(t=>t.id&&t.hasChildNodes()).map(t=>{const n=Number(t.tagName[1]);return{element:t,title:Ju(t),link:"#"+t.id,level:n}});return Qu(e,i)}function Ju(i){let e="";for(const t of i.childNodes)if(t.nodeType===1){if(Ku.test(t.className))continue;e+=t.textContent}else t.nodeType===3&&(e+=t.textContent);return e.trim()}function Qu(i,e){if(e===!1)return[];const t=(typeof e=="object"&&!Array.isArray(e)?e.level:e)||2,[n,s]=typeof t=="number"?[t,t]:t==="deep"?[2,6]:t;return nd(i,n,s)}function ed(i,e){const{isAsideEnabled:t}=Zu(),n=Nu(r,100);let s=null;zt(()=>{requestAnimationFrame(r),window.addEventListener("scroll",n)}),yu(()=>{a(location.hash)}),jn(()=>{window.removeEventListener("scroll",n)});function r(){if(!t.value)return;const o=window.scrollY,l=window.innerHeight,c=document.body.offsetHeight,u=Math.abs(o+l-c)<1,d=_a.map(({element:p,link:g})=>({link:g,top:td(p)})).filter(({top:p})=>!Number.isNaN(p)).sort((p,g)=>p.top-g.top);if(!d.length){a(null);return}if(o<1){a(null);return}if(u){a(d[d.length-1].link);return}let h=null;for(const{link:p,top:g}of d){if(g>o+bu()+4)break;h=p}a(h)}function a(o){s&&s.classList.remove("active"),o==null?s=null:s=i.value.querySelector(`a[href="${decodeURIComponent(o)}"]`);const l=s;l?(l.classList.add("active"),e.value.style.top=l.offsetTop+39+"px",e.value.style.opacity="1"):(e.value.style.top="33px",e.value.style.opacity="0")}}function td(i){let e=0;for(;i!==document.body;){if(i===null)return NaN;e+=i.offsetTop,i=i.offsetParent}return e}function nd(i,e,t){_a.length=0;const n=[],s=[];return i.forEach(r=>{const a={...r,children:[]};let o=s[s.length-1];for(;o&&o.level>=a.level;)s.pop(),o=s[s.length-1];if(a.element.classList.contains("ignore-header")||o&&"shouldIgnore"in o){s.push({level:a.level,shouldIgnore:!0});return}a.level>t||a.level<e||(_a.push({element:a.element,link:a.link}),o?o.children.push(a):n.push(a),s.push(a))}),n}const id=["href","title"],sd=Ce({__name:"VPDocOutlineItem",props:{headers:{},root:{type:Boolean}},setup(i){function e({target:t}){const n=t.href.split("#")[1],s=document.getElementById(decodeURIComponent(n));s==null||s.focus({preventScroll:!0})}return(t,n)=>{const s=fi("VPDocOutlineItem",!0);return L(),F("ul",{class:ct(["VPDocOutlineItem",t.root?"root":"nested"])},[(L(!0),F(tt,null,vt(t.headers,({children:r,link:a,title:o})=>(L(),F("li",null,[w("a",{class:"outline-link",href:a,onClick:e,title:o},xe(o),9,id),r!=null&&r.length?(L(),De(s,{key:0,headers:r},null,8,["headers"])):_e("",!0)]))),256))],2)}}}),vc=be(sd,[["__scopeId","data-v-b933a997"]]),rd={class:"content"},ad={"aria-level":"2",class:"outline-title",id:"doc-outline-aria-label",role:"heading"},od=Ce({__name:"VPDocAsideOutline",setup(i){const{frontmatter:e,theme:t}=Qe(),n=cc([]);fr(()=>{n.value=mo(e.value.outline??t.value.outline)});const s=We(),r=We();return ed(s,r),(a,o)=>(L(),F("nav",{"aria-labelledby":"doc-outline-aria-label",class:ct(["VPDocAsideOutline",{"has-outline":n.value.length>0}]),ref_key:"container",ref:s},[w("div",rd,[w("div",{class:"outline-marker",ref_key:"marker",ref:r},null,512),w("div",ad,xe(V(_c)(V(t))),1),Ie(vc,{headers:n.value,root:!0},null,8,["headers"])])],2))}}),ld=be(od,[["__scopeId","data-v-a5bbad30"]]),cd={class:"VPDocAsideCarbonAds"},ud=Ce({__name:"VPDocAsideCarbonAds",props:{carbonAds:{}},setup(i){const e=()=>null;return(t,n)=>(L(),F("div",cd,[Ie(V(e),{"carbon-ads":t.carbonAds},null,8,["carbon-ads"])]))}}),dd={class:"VPDocAside"},hd=Ce({__name:"VPDocAside",setup(i){const{theme:e}=Qe();return(t,n)=>(L(),F("div",dd,[J(t.$slots,"aside-top",{},void 0,!0),J(t.$slots,"aside-outline-before",{},void 0,!0),Ie(ld),J(t.$slots,"aside-outline-after",{},void 0,!0),n[0]||(n[0]=w("div",{class:"spacer"},null,-1)),J(t.$slots,"aside-ads-before",{},void 0,!0),V(e).carbonAds?(L(),De(ud,{key:0,"carbon-ads":V(e).carbonAds},null,8,["carbon-ads"])):_e("",!0),J(t.$slots,"aside-ads-after",{},void 0,!0),J(t.$slots,"aside-bottom",{},void 0,!0)]))}}),fd=be(hd,[["__scopeId","data-v-3f215769"]]);function pd(){const{theme:i,page:e}=Qe();return qe(()=>{const{text:t="Edit this page",pattern:n=""}=i.value.editLink||{};let s;return typeof n=="function"?s=n(e.value):s=n.replace(/:path/g,e.value.filePath),{url:s,text:t}})}function md(){const{page:i,theme:e,frontmatter:t}=Qe();return qe(()=>{var c,u,d,h,p,g,_,m;const n=gc(e.value.sidebar,i.value.relativePath),s=qu(n),r=gd(s,f=>f.link.replace(/[?#].*$/,"")),a=r.findIndex(f=>ci(i.value.relativePath,f.link)),o=((c=e.value.docFooter)==null?void 0:c.prev)===!1&&!t.value.prev||t.value.prev===!1,l=((u=e.value.docFooter)==null?void 0:u.next)===!1&&!t.value.next||t.value.next===!1;return{prev:o?void 0:{text:(typeof t.value.prev=="string"?t.value.prev:typeof t.value.prev=="object"?t.value.prev.text:void 0)??((d=r[a-1])==null?void 0:d.docFooterText)??((h=r[a-1])==null?void 0:h.text),link:(typeof t.value.prev=="object"?t.value.prev.link:void 0)??((p=r[a-1])==null?void 0:p.link)},next:l?void 0:{text:(typeof t.value.next=="string"?t.value.next:typeof t.value.next=="object"?t.value.next.text:void 0)??((g=r[a+1])==null?void 0:g.docFooterText)??((_=r[a+1])==null?void 0:_.text),link:(typeof t.value.next=="object"?t.value.next.link:void 0)??((m=r[a+1])==null?void 0:m.link)}}})}function gd(i,e){const t=new Set;return i.filter(n=>{const s=e(n);return t.has(s)?!1:t.add(s)})}const xn=Ce({__name:"VPLink",props:{tag:{},href:{},noIcon:{type:Boolean},target:{},rel:{}},setup(i){const e=i,t=qe(()=>e.tag??(e.href?"a":"span")),n=qe(()=>e.href&&uc.test(e.href)||e.target==="_blank");return(s,r)=>(L(),De(vn(t.value),{class:ct(["VPLink",{link:s.href,"vp-external-link-icon":n.value,"no-icon":s.noIcon}]),href:s.href?V(po)(s.href):void 0,target:s.target??(n.value?"_blank":void 0),rel:s.rel??(n.value?"noreferrer":void 0)},{default:te(()=>[J(s.$slots,"default")]),_:3},8,["class","href","target","rel"]))}}),_d={class:"VPLastUpdated"},vd=["datetime"],xd=Ce({__name:"VPDocFooterLastUpdated",setup(i){const{theme:e,page:t,lang:n}=Qe(),s=qe(()=>new Date(t.value.lastUpdated)),r=qe(()=>s.value.toISOString()),a=We("");return zt(()=>{hr(()=>{var o,l,c;a.value=new Intl.DateTimeFormat((l=(o=e.value.lastUpdated)==null?void 0:o.formatOptions)!=null&&l.forceLocale?n.value:void 0,((c=e.value.lastUpdated)==null?void 0:c.formatOptions)??{dateStyle:"short",timeStyle:"short"}).format(s.value)})}),(o,l)=>{var c;return L(),F("p",_d,[Xn(xe(((c=V(e).lastUpdated)==null?void 0:c.text)||V(e).lastUpdatedText||"Last updated")+": ",1),w("time",{datetime:r.value},xe(a.value),9,vd)])}}}),Md=be(xd,[["__scopeId","data-v-e98dd255"]]),Sd={key:0,class:"VPDocFooter"},yd={key:0,class:"edit-info"},bd={key:0,class:"edit-link"},Ed={key:1,class:"last-updated"},Td={key:1,class:"prev-next","aria-labelledby":"doc-footer-aria-label"},wd={class:"pager"},Ad=["innerHTML"],Rd=["innerHTML"],Cd={class:"pager"},Pd=["innerHTML"],Ld=["innerHTML"],Dd=Ce({__name:"VPDocFooter",setup(i){const{theme:e,page:t,frontmatter:n}=Qe(),s=pd(),r=md(),a=qe(()=>e.value.editLink&&n.value.editLink!==!1),o=qe(()=>t.value.lastUpdated),l=qe(()=>a.value||o.value||r.value.prev||r.value.next);return(c,u)=>{var d,h,p,g;return l.value?(L(),F("footer",Sd,[J(c.$slots,"doc-footer-before",{},void 0,!0),a.value||o.value?(L(),F("div",yd,[a.value?(L(),F("div",bd,[Ie(xn,{class:"edit-link-button",href:V(s).url,"no-icon":!0},{default:te(()=>[u[0]||(u[0]=w("span",{class:"vpi-square-pen edit-link-icon"},null,-1)),Xn(" "+xe(V(s).text),1)]),_:1,__:[0]},8,["href"])])):_e("",!0),o.value?(L(),F("div",Ed,[Ie(Md)])):_e("",!0)])):_e("",!0),(d=V(r).prev)!=null&&d.link||(h=V(r).next)!=null&&h.link?(L(),F("nav",Td,[u[1]||(u[1]=w("span",{class:"visually-hidden",id:"doc-footer-aria-label"},"Pager",-1)),w("div",wd,[(p=V(r).prev)!=null&&p.link?(L(),De(xn,{key:0,class:"pager-link prev",href:V(r).prev.link},{default:te(()=>{var _;return[w("span",{class:"desc",innerHTML:((_=V(e).docFooter)==null?void 0:_.prev)||"Previous page"},null,8,Ad),w("span",{class:"title",innerHTML:V(r).prev.text},null,8,Rd)]}),_:1},8,["href"])):_e("",!0)]),w("div",Cd,[(g=V(r).next)!=null&&g.link?(L(),De(xn,{key:0,class:"pager-link next",href:V(r).next.link},{default:te(()=>{var _;return[w("span",{class:"desc",innerHTML:((_=V(e).docFooter)==null?void 0:_.next)||"Next page"},null,8,Pd),w("span",{class:"title",innerHTML:V(r).next.text},null,8,Ld)]}),_:1},8,["href"])):_e("",!0)])])):_e("",!0)])):_e("",!0)}}}),Id=be(Dd,[["__scopeId","data-v-e257564d"]]),Ud={class:"container"},kd={class:"aside-container"},Nd={class:"aside-content"},Fd={class:"content"},Od={class:"content-container"},Bd={class:"main"},zd=Ce({__name:"VPDoc",setup(i){const{theme:e}=Qe(),t=pr(),{hasSidebar:n,hasAside:s,leftAside:r}=kn(),a=qe(()=>t.path.replace(/[./]+/g,"_").replace(/_html$/,""));return(o,l)=>{const c=fi("Content");return L(),F("div",{class:ct(["VPDoc",{"has-sidebar":V(n),"has-aside":V(s)}])},[J(o.$slots,"doc-top",{},void 0,!0),w("div",Ud,[V(s)?(L(),F("div",{key:0,class:ct(["aside",{"left-aside":V(r)}])},[l[0]||(l[0]=w("div",{class:"aside-curtain"},null,-1)),w("div",kd,[w("div",Nd,[Ie(fd,null,{"aside-top":te(()=>[J(o.$slots,"aside-top",{},void 0,!0)]),"aside-bottom":te(()=>[J(o.$slots,"aside-bottom",{},void 0,!0)]),"aside-outline-before":te(()=>[J(o.$slots,"aside-outline-before",{},void 0,!0)]),"aside-outline-after":te(()=>[J(o.$slots,"aside-outline-after",{},void 0,!0)]),"aside-ads-before":te(()=>[J(o.$slots,"aside-ads-before",{},void 0,!0)]),"aside-ads-after":te(()=>[J(o.$slots,"aside-ads-after",{},void 0,!0)]),_:3})])])],2)):_e("",!0),w("div",Fd,[w("div",Od,[J(o.$slots,"doc-before",{},void 0,!0),w("main",Bd,[Ie(c,{class:ct(["vp-doc",[a.value,V(e).externalLinkIcon&&"external-link-icon-enabled"]])},null,8,["class"])]),Ie(Id,null,{"doc-footer-before":te(()=>[J(o.$slots,"doc-footer-before",{},void 0,!0)]),_:3}),J(o.$slots,"doc-after",{},void 0,!0)])])]),J(o.$slots,"doc-bottom",{},void 0,!0)],2)}}}),Vd=be(zd,[["__scopeId","data-v-39a288b8"]]),Hd=Ce({__name:"VPButton",props:{tag:{},size:{default:"medium"},theme:{default:"brand"},text:{},href:{},target:{},rel:{}},setup(i){const e=i,t=qe(()=>e.href&&uc.test(e.href)),n=qe(()=>e.tag||(e.href?"a":"button"));return(s,r)=>(L(),De(vn(n.value),{class:ct(["VPButton",[s.size,s.theme]]),href:s.href?V(po)(s.href):void 0,target:e.target??(t.value?"_blank":void 0),rel:e.rel??(t.value?"noreferrer":void 0)},{default:te(()=>[Xn(xe(s.text),1)]),_:1},8,["class","href","target","rel"]))}}),Gd=be(Hd,[["__scopeId","data-v-fa7799d5"]]),Wd=["src","alt"],$d=Ce({inheritAttrs:!1,__name:"VPImage",props:{image:{},alt:{}},setup(i){return(e,t)=>{const n=fi("VPImage",!0);return e.image?(L(),F(tt,{key:0},[typeof e.image=="string"||"src"in e.image?(L(),F("img",Gn({key:0,class:"VPImage"},typeof e.image=="string"?e.$attrs:{...e.image,...e.$attrs},{src:V(Oi)(typeof e.image=="string"?e.image:e.image.src),alt:e.alt??(typeof e.image=="string"?"":e.image.alt||"")}),null,16,Wd)):(L(),F(tt,{key:1},[Ie(n,Gn({class:"dark",image:e.image.dark,alt:e.image.alt},e.$attrs),null,16,["image","alt"]),Ie(n,Gn({class:"light",image:e.image.light,alt:e.image.alt},e.$attrs),null,16,["image","alt"])],64))],64)):_e("",!0)}}}),ir=be($d,[["__scopeId","data-v-8426fc1a"]]),Xd={class:"container"},qd={class:"main"},Yd={class:"heading"},jd=["innerHTML"],Zd=["innerHTML"],Kd=["innerHTML"],Jd={key:0,class:"actions"},Qd={key:0,class:"image"},eh={class:"image-container"},th=Ce({__name:"VPHero",props:{name:{},text:{},tagline:{},image:{},actions:{}},setup(i){const e=mr("hero-image-slot-exists");return(t,n)=>(L(),F("div",{class:ct(["VPHero",{"has-image":t.image||V(e)}])},[w("div",Xd,[w("div",qd,[J(t.$slots,"home-hero-info-before",{},void 0,!0),J(t.$slots,"home-hero-info",{},()=>[w("h1",Yd,[t.name?(L(),F("span",{key:0,innerHTML:t.name,class:"name clip"},null,8,jd)):_e("",!0),t.text?(L(),F("span",{key:1,innerHTML:t.text,class:"text"},null,8,Zd)):_e("",!0)]),t.tagline?(L(),F("p",{key:0,innerHTML:t.tagline,class:"tagline"},null,8,Kd)):_e("",!0)],!0),J(t.$slots,"home-hero-info-after",{},void 0,!0),t.actions?(L(),F("div",Jd,[(L(!0),F(tt,null,vt(t.actions,s=>(L(),F("div",{key:s.link,class:"action"},[Ie(Gd,{tag:"a",size:"medium",theme:s.theme,text:s.text,href:s.link,target:s.target,rel:s.rel},null,8,["theme","text","href","target","rel"])]))),128))])):_e("",!0),J(t.$slots,"home-hero-actions-after",{},void 0,!0)]),t.image||V(e)?(L(),F("div",Qd,[w("div",eh,[n[0]||(n[0]=w("div",{class:"image-bg"},null,-1)),J(t.$slots,"home-hero-image",{},()=>[t.image?(L(),De(ir,{key:0,class:"image-src",image:t.image},null,8,["image"])):_e("",!0)],!0)])])):_e("",!0)])],2))}}),nh=be(th,[["__scopeId","data-v-4f9c455b"]]);/**
 * @license
 * Copyright 2010-2025 Three.js Authors
 * SPDX-License-Identifier: MIT
 */const go="179",ih=0,el=1,sh=2,xc=1,_o=2,Rn=3,qn=0,Yt=1,qt=2,Wn=0,ki=1,sr=2,tl=3,nl=4,rh=5,ai=100,ah=101,oh=102,lh=103,ch=104,uh=200,dh=201,hh=202,fh=203,va=204,xa=205,ph=206,mh=207,gh=208,_h=209,vh=210,xh=211,Mh=212,Sh=213,yh=214,Ma=0,Sa=1,ya=2,Bi=3,ba=4,Ea=5,Ta=6,wa=7,vo=0,bh=1,Eh=2,$n=0,Th=1,wh=2,Ah=3,Rh=4,Ch=5,Ph=6,Lh=7,Mc=300,zi=301,Vi=302,Aa=303,Ra=304,_r=306,Ca=1e3,Cn=1001,Pa=1002,fn=1003,Dh=1004,ys=1005,an=1006,Rr=1007,li=1008,Mn=1009,Sc=1010,yc=1011,as=1012,xo=1013,ui=1014,Pn=1015,ps=1016,Mo=1017,So=1018,os=1020,bc=35902,Ec=1021,Tc=1022,hn=1023,ls=1026,cs=1027,wc=1028,yo=1029,Ac=1030,bo=1031,Eo=1033,js=33776,Zs=33777,Ks=33778,Js=33779,La=35840,Da=35841,Ia=35842,Ua=35843,ka=36196,Na=37492,Fa=37496,Oa=37808,Ba=37809,za=37810,Va=37811,Ha=37812,Ga=37813,Wa=37814,$a=37815,Xa=37816,qa=37817,Ya=37818,ja=37819,Za=37820,Ka=37821,Qs=36492,Ja=36494,Qa=36495,Rc=36283,eo=36284,to=36285,no=36286,Ih=3200,Uh=3201,Cc=0,kh=1,Hn="",rn="srgb",Hi="srgb-linear",rr="linear",mt="srgb",_i=7680,il=519,Nh=512,Fh=513,Oh=514,Pc=515,Bh=516,zh=517,Vh=518,Hh=519,sl=35044,rl="300 es",_n=2e3,ar=2001;class Wi{addEventListener(e,t){this._listeners===void 0&&(this._listeners={});const n=this._listeners;n[e]===void 0&&(n[e]=[]),n[e].indexOf(t)===-1&&n[e].push(t)}hasEventListener(e,t){const n=this._listeners;return n===void 0?!1:n[e]!==void 0&&n[e].indexOf(t)!==-1}removeEventListener(e,t){const n=this._listeners;if(n===void 0)return;const s=n[e];if(s!==void 0){const r=s.indexOf(t);r!==-1&&s.splice(r,1)}}dispatchEvent(e){const t=this._listeners;if(t===void 0)return;const n=t[e.type];if(n!==void 0){e.target=this;const s=n.slice(0);for(let r=0,a=s.length;r<a;r++)s[r].call(this,e);e.target=null}}}const Ft=["00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f","10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f","20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f","30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f","40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f","50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f","60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f","70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f","80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f","90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f","a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af","b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf","c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf","d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df","e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef","f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"];let al=1234567;const is=Math.PI/180,us=180/Math.PI;function $i(){const i=Math.random()*4294967295|0,e=Math.random()*4294967295|0,t=Math.random()*4294967295|0,n=Math.random()*4294967295|0;return(Ft[i&255]+Ft[i>>8&255]+Ft[i>>16&255]+Ft[i>>24&255]+"-"+Ft[e&255]+Ft[e>>8&255]+"-"+Ft[e>>16&15|64]+Ft[e>>24&255]+"-"+Ft[t&63|128]+Ft[t>>8&255]+"-"+Ft[t>>16&255]+Ft[t>>24&255]+Ft[n&255]+Ft[n>>8&255]+Ft[n>>16&255]+Ft[n>>24&255]).toLowerCase()}function it(i,e,t){return Math.max(e,Math.min(t,i))}function To(i,e){return(i%e+e)%e}function Gh(i,e,t,n,s){return n+(i-e)*(s-n)/(t-e)}function Wh(i,e,t){return i!==e?(t-i)/(e-i):0}function ss(i,e,t){return(1-t)*i+t*e}function $h(i,e,t,n){return ss(i,e,1-Math.exp(-t*n))}function Xh(i,e=1){return e-Math.abs(To(i,e*2)-e)}function qh(i,e,t){return i<=e?0:i>=t?1:(i=(i-e)/(t-e),i*i*(3-2*i))}function Yh(i,e,t){return i<=e?0:i>=t?1:(i=(i-e)/(t-e),i*i*i*(i*(i*6-15)+10))}function jh(i,e){return i+Math.floor(Math.random()*(e-i+1))}function Zh(i,e){return i+Math.random()*(e-i)}function Kh(i){return i*(.5-Math.random())}function Jh(i){i!==void 0&&(al=i);let e=al+=1831565813;return e=Math.imul(e^e>>>15,e|1),e^=e+Math.imul(e^e>>>7,e|61),((e^e>>>14)>>>0)/4294967296}function Qh(i){return i*is}function ef(i){return i*us}function tf(i){return(i&i-1)===0&&i!==0}function nf(i){return Math.pow(2,Math.ceil(Math.log(i)/Math.LN2))}function sf(i){return Math.pow(2,Math.floor(Math.log(i)/Math.LN2))}function rf(i,e,t,n,s){const r=Math.cos,a=Math.sin,o=r(t/2),l=a(t/2),c=r((e+n)/2),u=a((e+n)/2),d=r((e-n)/2),h=a((e-n)/2),p=r((n-e)/2),g=a((n-e)/2);switch(s){case"XYX":i.set(o*u,l*d,l*h,o*c);break;case"YZY":i.set(l*h,o*u,l*d,o*c);break;case"ZXZ":i.set(l*d,l*h,o*u,o*c);break;case"XZX":i.set(o*u,l*g,l*p,o*c);break;case"YXY":i.set(l*p,o*u,l*g,o*c);break;case"ZYZ":i.set(l*g,l*p,o*u,o*c);break;default:console.warn("THREE.MathUtils: .setQuaternionFromProperEuler() encountered an unknown order: "+s)}}function Di(i,e){switch(e.constructor){case Float32Array:return i;case Uint32Array:return i/4294967295;case Uint16Array:return i/65535;case Uint8Array:return i/255;case Int32Array:return Math.max(i/2147483647,-1);case Int16Array:return Math.max(i/32767,-1);case Int8Array:return Math.max(i/127,-1);default:throw new Error("Invalid component type.")}}function Ht(i,e){switch(e.constructor){case Float32Array:return i;case Uint32Array:return Math.round(i*4294967295);case Uint16Array:return Math.round(i*65535);case Uint8Array:return Math.round(i*255);case Int32Array:return Math.round(i*2147483647);case Int16Array:return Math.round(i*32767);case Int8Array:return Math.round(i*127);default:throw new Error("Invalid component type.")}}const Cr={DEG2RAD:is,RAD2DEG:us,generateUUID:$i,clamp:it,euclideanModulo:To,mapLinear:Gh,inverseLerp:Wh,lerp:ss,damp:$h,pingpong:Xh,smoothstep:qh,smootherstep:Yh,randInt:jh,randFloat:Zh,randFloatSpread:Kh,seededRandom:Jh,degToRad:Qh,radToDeg:ef,isPowerOfTwo:tf,ceilPowerOfTwo:nf,floorPowerOfTwo:sf,setQuaternionFromProperEuler:rf,normalize:Ht,denormalize:Di};class et{constructor(e=0,t=0){et.prototype.isVector2=!0,this.x=e,this.y=t}get width(){return this.x}set width(e){this.x=e}get height(){return this.y}set height(e){this.y=e}set(e,t){return this.x=e,this.y=t,this}setScalar(e){return this.x=e,this.y=e,this}setX(e){return this.x=e,this}setY(e){return this.y=e,this}setComponent(e,t){switch(e){case 0:this.x=t;break;case 1:this.y=t;break;default:throw new Error("index is out of range: "+e)}return this}getComponent(e){switch(e){case 0:return this.x;case 1:return this.y;default:throw new Error("index is out of range: "+e)}}clone(){return new this.constructor(this.x,this.y)}copy(e){return this.x=e.x,this.y=e.y,this}add(e){return this.x+=e.x,this.y+=e.y,this}addScalar(e){return this.x+=e,this.y+=e,this}addVectors(e,t){return this.x=e.x+t.x,this.y=e.y+t.y,this}addScaledVector(e,t){return this.x+=e.x*t,this.y+=e.y*t,this}sub(e){return this.x-=e.x,this.y-=e.y,this}subScalar(e){return this.x-=e,this.y-=e,this}subVectors(e,t){return this.x=e.x-t.x,this.y=e.y-t.y,this}multiply(e){return this.x*=e.x,this.y*=e.y,this}multiplyScalar(e){return this.x*=e,this.y*=e,this}divide(e){return this.x/=e.x,this.y/=e.y,this}divideScalar(e){return this.multiplyScalar(1/e)}applyMatrix3(e){const t=this.x,n=this.y,s=e.elements;return this.x=s[0]*t+s[3]*n+s[6],this.y=s[1]*t+s[4]*n+s[7],this}min(e){return this.x=Math.min(this.x,e.x),this.y=Math.min(this.y,e.y),this}max(e){return this.x=Math.max(this.x,e.x),this.y=Math.max(this.y,e.y),this}clamp(e,t){return this.x=it(this.x,e.x,t.x),this.y=it(this.y,e.y,t.y),this}clampScalar(e,t){return this.x=it(this.x,e,t),this.y=it(this.y,e,t),this}clampLength(e,t){const n=this.length();return this.divideScalar(n||1).multiplyScalar(it(n,e,t))}floor(){return this.x=Math.floor(this.x),this.y=Math.floor(this.y),this}ceil(){return this.x=Math.ceil(this.x),this.y=Math.ceil(this.y),this}round(){return this.x=Math.round(this.x),this.y=Math.round(this.y),this}roundToZero(){return this.x=Math.trunc(this.x),this.y=Math.trunc(this.y),this}negate(){return this.x=-this.x,this.y=-this.y,this}dot(e){return this.x*e.x+this.y*e.y}cross(e){return this.x*e.y-this.y*e.x}lengthSq(){return this.x*this.x+this.y*this.y}length(){return Math.sqrt(this.x*this.x+this.y*this.y)}manhattanLength(){return Math.abs(this.x)+Math.abs(this.y)}normalize(){return this.divideScalar(this.length()||1)}angle(){return Math.atan2(-this.y,-this.x)+Math.PI}angleTo(e){const t=Math.sqrt(this.lengthSq()*e.lengthSq());if(t===0)return Math.PI/2;const n=this.dot(e)/t;return Math.acos(it(n,-1,1))}distanceTo(e){return Math.sqrt(this.distanceToSquared(e))}distanceToSquared(e){const t=this.x-e.x,n=this.y-e.y;return t*t+n*n}manhattanDistanceTo(e){return Math.abs(this.x-e.x)+Math.abs(this.y-e.y)}setLength(e){return this.normalize().multiplyScalar(e)}lerp(e,t){return this.x+=(e.x-this.x)*t,this.y+=(e.y-this.y)*t,this}lerpVectors(e,t,n){return this.x=e.x+(t.x-e.x)*n,this.y=e.y+(t.y-e.y)*n,this}equals(e){return e.x===this.x&&e.y===this.y}fromArray(e,t=0){return this.x=e[t],this.y=e[t+1],this}toArray(e=[],t=0){return e[t]=this.x,e[t+1]=this.y,e}fromBufferAttribute(e,t){return this.x=e.getX(t),this.y=e.getY(t),this}rotateAround(e,t){const n=Math.cos(t),s=Math.sin(t),r=this.x-e.x,a=this.y-e.y;return this.x=r*n-a*s+e.x,this.y=r*s+a*n+e.y,this}random(){return this.x=Math.random(),this.y=Math.random(),this}*[Symbol.iterator](){yield this.x,yield this.y}}class ms{constructor(e=0,t=0,n=0,s=1){this.isQuaternion=!0,this._x=e,this._y=t,this._z=n,this._w=s}static slerpFlat(e,t,n,s,r,a,o){let l=n[s+0],c=n[s+1],u=n[s+2],d=n[s+3];const h=r[a+0],p=r[a+1],g=r[a+2],_=r[a+3];if(o===0){e[t+0]=l,e[t+1]=c,e[t+2]=u,e[t+3]=d;return}if(o===1){e[t+0]=h,e[t+1]=p,e[t+2]=g,e[t+3]=_;return}if(d!==_||l!==h||c!==p||u!==g){let m=1-o;const f=l*h+c*p+u*g+d*_,E=f>=0?1:-1,b=1-f*f;if(b>Number.EPSILON){const T=Math.sqrt(b),A=Math.atan2(T,f*E);m=Math.sin(m*A)/T,o=Math.sin(o*A)/T}const v=o*E;if(l=l*m+h*v,c=c*m+p*v,u=u*m+g*v,d=d*m+_*v,m===1-o){const T=1/Math.sqrt(l*l+c*c+u*u+d*d);l*=T,c*=T,u*=T,d*=T}}e[t]=l,e[t+1]=c,e[t+2]=u,e[t+3]=d}static multiplyQuaternionsFlat(e,t,n,s,r,a){const o=n[s],l=n[s+1],c=n[s+2],u=n[s+3],d=r[a],h=r[a+1],p=r[a+2],g=r[a+3];return e[t]=o*g+u*d+l*p-c*h,e[t+1]=l*g+u*h+c*d-o*p,e[t+2]=c*g+u*p+o*h-l*d,e[t+3]=u*g-o*d-l*h-c*p,e}get x(){return this._x}set x(e){this._x=e,this._onChangeCallback()}get y(){return this._y}set y(e){this._y=e,this._onChangeCallback()}get z(){return this._z}set z(e){this._z=e,this._onChangeCallback()}get w(){return this._w}set w(e){this._w=e,this._onChangeCallback()}set(e,t,n,s){return this._x=e,this._y=t,this._z=n,this._w=s,this._onChangeCallback(),this}clone(){return new this.constructor(this._x,this._y,this._z,this._w)}copy(e){return this._x=e.x,this._y=e.y,this._z=e.z,this._w=e.w,this._onChangeCallback(),this}setFromEuler(e,t=!0){const n=e._x,s=e._y,r=e._z,a=e._order,o=Math.cos,l=Math.sin,c=o(n/2),u=o(s/2),d=o(r/2),h=l(n/2),p=l(s/2),g=l(r/2);switch(a){case"XYZ":this._x=h*u*d+c*p*g,this._y=c*p*d-h*u*g,this._z=c*u*g+h*p*d,this._w=c*u*d-h*p*g;break;case"YXZ":this._x=h*u*d+c*p*g,this._y=c*p*d-h*u*g,this._z=c*u*g-h*p*d,this._w=c*u*d+h*p*g;break;case"ZXY":this._x=h*u*d-c*p*g,this._y=c*p*d+h*u*g,this._z=c*u*g+h*p*d,this._w=c*u*d-h*p*g;break;case"ZYX":this._x=h*u*d-c*p*g,this._y=c*p*d+h*u*g,this._z=c*u*g-h*p*d,this._w=c*u*d+h*p*g;break;case"YZX":this._x=h*u*d+c*p*g,this._y=c*p*d+h*u*g,this._z=c*u*g-h*p*d,this._w=c*u*d-h*p*g;break;case"XZY":this._x=h*u*d-c*p*g,this._y=c*p*d-h*u*g,this._z=c*u*g+h*p*d,this._w=c*u*d+h*p*g;break;default:console.warn("THREE.Quaternion: .setFromEuler() encountered an unknown order: "+a)}return t===!0&&this._onChangeCallback(),this}setFromAxisAngle(e,t){const n=t/2,s=Math.sin(n);return this._x=e.x*s,this._y=e.y*s,this._z=e.z*s,this._w=Math.cos(n),this._onChangeCallback(),this}setFromRotationMatrix(e){const t=e.elements,n=t[0],s=t[4],r=t[8],a=t[1],o=t[5],l=t[9],c=t[2],u=t[6],d=t[10],h=n+o+d;if(h>0){const p=.5/Math.sqrt(h+1);this._w=.25/p,this._x=(u-l)*p,this._y=(r-c)*p,this._z=(a-s)*p}else if(n>o&&n>d){const p=2*Math.sqrt(1+n-o-d);this._w=(u-l)/p,this._x=.25*p,this._y=(s+a)/p,this._z=(r+c)/p}else if(o>d){const p=2*Math.sqrt(1+o-n-d);this._w=(r-c)/p,this._x=(s+a)/p,this._y=.25*p,this._z=(l+u)/p}else{const p=2*Math.sqrt(1+d-n-o);this._w=(a-s)/p,this._x=(r+c)/p,this._y=(l+u)/p,this._z=.25*p}return this._onChangeCallback(),this}setFromUnitVectors(e,t){let n=e.dot(t)+1;return n<1e-8?(n=0,Math.abs(e.x)>Math.abs(e.z)?(this._x=-e.y,this._y=e.x,this._z=0,this._w=n):(this._x=0,this._y=-e.z,this._z=e.y,this._w=n)):(this._x=e.y*t.z-e.z*t.y,this._y=e.z*t.x-e.x*t.z,this._z=e.x*t.y-e.y*t.x,this._w=n),this.normalize()}angleTo(e){return 2*Math.acos(Math.abs(it(this.dot(e),-1,1)))}rotateTowards(e,t){const n=this.angleTo(e);if(n===0)return this;const s=Math.min(1,t/n);return this.slerp(e,s),this}identity(){return this.set(0,0,0,1)}invert(){return this.conjugate()}conjugate(){return this._x*=-1,this._y*=-1,this._z*=-1,this._onChangeCallback(),this}dot(e){return this._x*e._x+this._y*e._y+this._z*e._z+this._w*e._w}lengthSq(){return this._x*this._x+this._y*this._y+this._z*this._z+this._w*this._w}length(){return Math.sqrt(this._x*this._x+this._y*this._y+this._z*this._z+this._w*this._w)}normalize(){let e=this.length();return e===0?(this._x=0,this._y=0,this._z=0,this._w=1):(e=1/e,this._x=this._x*e,this._y=this._y*e,this._z=this._z*e,this._w=this._w*e),this._onChangeCallback(),this}multiply(e){return this.multiplyQuaternions(this,e)}premultiply(e){return this.multiplyQuaternions(e,this)}multiplyQuaternions(e,t){const n=e._x,s=e._y,r=e._z,a=e._w,o=t._x,l=t._y,c=t._z,u=t._w;return this._x=n*u+a*o+s*c-r*l,this._y=s*u+a*l+r*o-n*c,this._z=r*u+a*c+n*l-s*o,this._w=a*u-n*o-s*l-r*c,this._onChangeCallback(),this}slerp(e,t){if(t===0)return this;if(t===1)return this.copy(e);const n=this._x,s=this._y,r=this._z,a=this._w;let o=a*e._w+n*e._x+s*e._y+r*e._z;if(o<0?(this._w=-e._w,this._x=-e._x,this._y=-e._y,this._z=-e._z,o=-o):this.copy(e),o>=1)return this._w=a,this._x=n,this._y=s,this._z=r,this;const l=1-o*o;if(l<=Number.EPSILON){const p=1-t;return this._w=p*a+t*this._w,this._x=p*n+t*this._x,this._y=p*s+t*this._y,this._z=p*r+t*this._z,this.normalize(),this}const c=Math.sqrt(l),u=Math.atan2(c,o),d=Math.sin((1-t)*u)/c,h=Math.sin(t*u)/c;return this._w=a*d+this._w*h,this._x=n*d+this._x*h,this._y=s*d+this._y*h,this._z=r*d+this._z*h,this._onChangeCallback(),this}slerpQuaternions(e,t,n){return this.copy(e).slerp(t,n)}random(){const e=2*Math.PI*Math.random(),t=2*Math.PI*Math.random(),n=Math.random(),s=Math.sqrt(1-n),r=Math.sqrt(n);return this.set(s*Math.sin(e),s*Math.cos(e),r*Math.sin(t),r*Math.cos(t))}equals(e){return e._x===this._x&&e._y===this._y&&e._z===this._z&&e._w===this._w}fromArray(e,t=0){return this._x=e[t],this._y=e[t+1],this._z=e[t+2],this._w=e[t+3],this._onChangeCallback(),this}toArray(e=[],t=0){return e[t]=this._x,e[t+1]=this._y,e[t+2]=this._z,e[t+3]=this._w,e}fromBufferAttribute(e,t){return this._x=e.getX(t),this._y=e.getY(t),this._z=e.getZ(t),this._w=e.getW(t),this._onChangeCallback(),this}toJSON(){return this.toArray()}_onChange(e){return this._onChangeCallback=e,this}_onChangeCallback(){}*[Symbol.iterator](){yield this._x,yield this._y,yield this._z,yield this._w}}class O{constructor(e=0,t=0,n=0){O.prototype.isVector3=!0,this.x=e,this.y=t,this.z=n}set(e,t,n){return n===void 0&&(n=this.z),this.x=e,this.y=t,this.z=n,this}setScalar(e){return this.x=e,this.y=e,this.z=e,this}setX(e){return this.x=e,this}setY(e){return this.y=e,this}setZ(e){return this.z=e,this}setComponent(e,t){switch(e){case 0:this.x=t;break;case 1:this.y=t;break;case 2:this.z=t;break;default:throw new Error("index is out of range: "+e)}return this}getComponent(e){switch(e){case 0:return this.x;case 1:return this.y;case 2:return this.z;default:throw new Error("index is out of range: "+e)}}clone(){return new this.constructor(this.x,this.y,this.z)}copy(e){return this.x=e.x,this.y=e.y,this.z=e.z,this}add(e){return this.x+=e.x,this.y+=e.y,this.z+=e.z,this}addScalar(e){return this.x+=e,this.y+=e,this.z+=e,this}addVectors(e,t){return this.x=e.x+t.x,this.y=e.y+t.y,this.z=e.z+t.z,this}addScaledVector(e,t){return this.x+=e.x*t,this.y+=e.y*t,this.z+=e.z*t,this}sub(e){return this.x-=e.x,this.y-=e.y,this.z-=e.z,this}subScalar(e){return this.x-=e,this.y-=e,this.z-=e,this}subVectors(e,t){return this.x=e.x-t.x,this.y=e.y-t.y,this.z=e.z-t.z,this}multiply(e){return this.x*=e.x,this.y*=e.y,this.z*=e.z,this}multiplyScalar(e){return this.x*=e,this.y*=e,this.z*=e,this}multiplyVectors(e,t){return this.x=e.x*t.x,this.y=e.y*t.y,this.z=e.z*t.z,this}applyEuler(e){return this.applyQuaternion(ol.setFromEuler(e))}applyAxisAngle(e,t){return this.applyQuaternion(ol.setFromAxisAngle(e,t))}applyMatrix3(e){const t=this.x,n=this.y,s=this.z,r=e.elements;return this.x=r[0]*t+r[3]*n+r[6]*s,this.y=r[1]*t+r[4]*n+r[7]*s,this.z=r[2]*t+r[5]*n+r[8]*s,this}applyNormalMatrix(e){return this.applyMatrix3(e).normalize()}applyMatrix4(e){const t=this.x,n=this.y,s=this.z,r=e.elements,a=1/(r[3]*t+r[7]*n+r[11]*s+r[15]);return this.x=(r[0]*t+r[4]*n+r[8]*s+r[12])*a,this.y=(r[1]*t+r[5]*n+r[9]*s+r[13])*a,this.z=(r[2]*t+r[6]*n+r[10]*s+r[14])*a,this}applyQuaternion(e){const t=this.x,n=this.y,s=this.z,r=e.x,a=e.y,o=e.z,l=e.w,c=2*(a*s-o*n),u=2*(o*t-r*s),d=2*(r*n-a*t);return this.x=t+l*c+a*d-o*u,this.y=n+l*u+o*c-r*d,this.z=s+l*d+r*u-a*c,this}project(e){return this.applyMatrix4(e.matrixWorldInverse).applyMatrix4(e.projectionMatrix)}unproject(e){return this.applyMatrix4(e.projectionMatrixInverse).applyMatrix4(e.matrixWorld)}transformDirection(e){const t=this.x,n=this.y,s=this.z,r=e.elements;return this.x=r[0]*t+r[4]*n+r[8]*s,this.y=r[1]*t+r[5]*n+r[9]*s,this.z=r[2]*t+r[6]*n+r[10]*s,this.normalize()}divide(e){return this.x/=e.x,this.y/=e.y,this.z/=e.z,this}divideScalar(e){return this.multiplyScalar(1/e)}min(e){return this.x=Math.min(this.x,e.x),this.y=Math.min(this.y,e.y),this.z=Math.min(this.z,e.z),this}max(e){return this.x=Math.max(this.x,e.x),this.y=Math.max(this.y,e.y),this.z=Math.max(this.z,e.z),this}clamp(e,t){return this.x=it(this.x,e.x,t.x),this.y=it(this.y,e.y,t.y),this.z=it(this.z,e.z,t.z),this}clampScalar(e,t){return this.x=it(this.x,e,t),this.y=it(this.y,e,t),this.z=it(this.z,e,t),this}clampLength(e,t){const n=this.length();return this.divideScalar(n||1).multiplyScalar(it(n,e,t))}floor(){return this.x=Math.floor(this.x),this.y=Math.floor(this.y),this.z=Math.floor(this.z),this}ceil(){return this.x=Math.ceil(this.x),this.y=Math.ceil(this.y),this.z=Math.ceil(this.z),this}round(){return this.x=Math.round(this.x),this.y=Math.round(this.y),this.z=Math.round(this.z),this}roundToZero(){return this.x=Math.trunc(this.x),this.y=Math.trunc(this.y),this.z=Math.trunc(this.z),this}negate(){return this.x=-this.x,this.y=-this.y,this.z=-this.z,this}dot(e){return this.x*e.x+this.y*e.y+this.z*e.z}lengthSq(){return this.x*this.x+this.y*this.y+this.z*this.z}length(){return Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z)}manhattanLength(){return Math.abs(this.x)+Math.abs(this.y)+Math.abs(this.z)}normalize(){return this.divideScalar(this.length()||1)}setLength(e){return this.normalize().multiplyScalar(e)}lerp(e,t){return this.x+=(e.x-this.x)*t,this.y+=(e.y-this.y)*t,this.z+=(e.z-this.z)*t,this}lerpVectors(e,t,n){return this.x=e.x+(t.x-e.x)*n,this.y=e.y+(t.y-e.y)*n,this.z=e.z+(t.z-e.z)*n,this}cross(e){return this.crossVectors(this,e)}crossVectors(e,t){const n=e.x,s=e.y,r=e.z,a=t.x,o=t.y,l=t.z;return this.x=s*l-r*o,this.y=r*a-n*l,this.z=n*o-s*a,this}projectOnVector(e){const t=e.lengthSq();if(t===0)return this.set(0,0,0);const n=e.dot(this)/t;return this.copy(e).multiplyScalar(n)}projectOnPlane(e){return Pr.copy(this).projectOnVector(e),this.sub(Pr)}reflect(e){return this.sub(Pr.copy(e).multiplyScalar(2*this.dot(e)))}angleTo(e){const t=Math.sqrt(this.lengthSq()*e.lengthSq());if(t===0)return Math.PI/2;const n=this.dot(e)/t;return Math.acos(it(n,-1,1))}distanceTo(e){return Math.sqrt(this.distanceToSquared(e))}distanceToSquared(e){const t=this.x-e.x,n=this.y-e.y,s=this.z-e.z;return t*t+n*n+s*s}manhattanDistanceTo(e){return Math.abs(this.x-e.x)+Math.abs(this.y-e.y)+Math.abs(this.z-e.z)}setFromSpherical(e){return this.setFromSphericalCoords(e.radius,e.phi,e.theta)}setFromSphericalCoords(e,t,n){const s=Math.sin(t)*e;return this.x=s*Math.sin(n),this.y=Math.cos(t)*e,this.z=s*Math.cos(n),this}setFromCylindrical(e){return this.setFromCylindricalCoords(e.radius,e.theta,e.y)}setFromCylindricalCoords(e,t,n){return this.x=e*Math.sin(t),this.y=n,this.z=e*Math.cos(t),this}setFromMatrixPosition(e){const t=e.elements;return this.x=t[12],this.y=t[13],this.z=t[14],this}setFromMatrixScale(e){const t=this.setFromMatrixColumn(e,0).length(),n=this.setFromMatrixColumn(e,1).length(),s=this.setFromMatrixColumn(e,2).length();return this.x=t,this.y=n,this.z=s,this}setFromMatrixColumn(e,t){return this.fromArray(e.elements,t*4)}setFromMatrix3Column(e,t){return this.fromArray(e.elements,t*3)}setFromEuler(e){return this.x=e._x,this.y=e._y,this.z=e._z,this}setFromColor(e){return this.x=e.r,this.y=e.g,this.z=e.b,this}equals(e){return e.x===this.x&&e.y===this.y&&e.z===this.z}fromArray(e,t=0){return this.x=e[t],this.y=e[t+1],this.z=e[t+2],this}toArray(e=[],t=0){return e[t]=this.x,e[t+1]=this.y,e[t+2]=this.z,e}fromBufferAttribute(e,t){return this.x=e.getX(t),this.y=e.getY(t),this.z=e.getZ(t),this}random(){return this.x=Math.random(),this.y=Math.random(),this.z=Math.random(),this}randomDirection(){const e=Math.random()*Math.PI*2,t=Math.random()*2-1,n=Math.sqrt(1-t*t);return this.x=n*Math.cos(e),this.y=t,this.z=n*Math.sin(e),this}*[Symbol.iterator](){yield this.x,yield this.y,yield this.z}}const Pr=new O,ol=new ms;class Ze{constructor(e,t,n,s,r,a,o,l,c){Ze.prototype.isMatrix3=!0,this.elements=[1,0,0,0,1,0,0,0,1],e!==void 0&&this.set(e,t,n,s,r,a,o,l,c)}set(e,t,n,s,r,a,o,l,c){const u=this.elements;return u[0]=e,u[1]=s,u[2]=o,u[3]=t,u[4]=r,u[5]=l,u[6]=n,u[7]=a,u[8]=c,this}identity(){return this.set(1,0,0,0,1,0,0,0,1),this}copy(e){const t=this.elements,n=e.elements;return t[0]=n[0],t[1]=n[1],t[2]=n[2],t[3]=n[3],t[4]=n[4],t[5]=n[5],t[6]=n[6],t[7]=n[7],t[8]=n[8],this}extractBasis(e,t,n){return e.setFromMatrix3Column(this,0),t.setFromMatrix3Column(this,1),n.setFromMatrix3Column(this,2),this}setFromMatrix4(e){const t=e.elements;return this.set(t[0],t[4],t[8],t[1],t[5],t[9],t[2],t[6],t[10]),this}multiply(e){return this.multiplyMatrices(this,e)}premultiply(e){return this.multiplyMatrices(e,this)}multiplyMatrices(e,t){const n=e.elements,s=t.elements,r=this.elements,a=n[0],o=n[3],l=n[6],c=n[1],u=n[4],d=n[7],h=n[2],p=n[5],g=n[8],_=s[0],m=s[3],f=s[6],E=s[1],b=s[4],v=s[7],T=s[2],A=s[5],C=s[8];return r[0]=a*_+o*E+l*T,r[3]=a*m+o*b+l*A,r[6]=a*f+o*v+l*C,r[1]=c*_+u*E+d*T,r[4]=c*m+u*b+d*A,r[7]=c*f+u*v+d*C,r[2]=h*_+p*E+g*T,r[5]=h*m+p*b+g*A,r[8]=h*f+p*v+g*C,this}multiplyScalar(e){const t=this.elements;return t[0]*=e,t[3]*=e,t[6]*=e,t[1]*=e,t[4]*=e,t[7]*=e,t[2]*=e,t[5]*=e,t[8]*=e,this}determinant(){const e=this.elements,t=e[0],n=e[1],s=e[2],r=e[3],a=e[4],o=e[5],l=e[6],c=e[7],u=e[8];return t*a*u-t*o*c-n*r*u+n*o*l+s*r*c-s*a*l}invert(){const e=this.elements,t=e[0],n=e[1],s=e[2],r=e[3],a=e[4],o=e[5],l=e[6],c=e[7],u=e[8],d=u*a-o*c,h=o*l-u*r,p=c*r-a*l,g=t*d+n*h+s*p;if(g===0)return this.set(0,0,0,0,0,0,0,0,0);const _=1/g;return e[0]=d*_,e[1]=(s*c-u*n)*_,e[2]=(o*n-s*a)*_,e[3]=h*_,e[4]=(u*t-s*l)*_,e[5]=(s*r-o*t)*_,e[6]=p*_,e[7]=(n*l-c*t)*_,e[8]=(a*t-n*r)*_,this}transpose(){let e;const t=this.elements;return e=t[1],t[1]=t[3],t[3]=e,e=t[2],t[2]=t[6],t[6]=e,e=t[5],t[5]=t[7],t[7]=e,this}getNormalMatrix(e){return this.setFromMatrix4(e).invert().transpose()}transposeIntoArray(e){const t=this.elements;return e[0]=t[0],e[1]=t[3],e[2]=t[6],e[3]=t[1],e[4]=t[4],e[5]=t[7],e[6]=t[2],e[7]=t[5],e[8]=t[8],this}setUvTransform(e,t,n,s,r,a,o){const l=Math.cos(r),c=Math.sin(r);return this.set(n*l,n*c,-n*(l*a+c*o)+a+e,-s*c,s*l,-s*(-c*a+l*o)+o+t,0,0,1),this}scale(e,t){return this.premultiply(Lr.makeScale(e,t)),this}rotate(e){return this.premultiply(Lr.makeRotation(-e)),this}translate(e,t){return this.premultiply(Lr.makeTranslation(e,t)),this}makeTranslation(e,t){return e.isVector2?this.set(1,0,e.x,0,1,e.y,0,0,1):this.set(1,0,e,0,1,t,0,0,1),this}makeRotation(e){const t=Math.cos(e),n=Math.sin(e);return this.set(t,-n,0,n,t,0,0,0,1),this}makeScale(e,t){return this.set(e,0,0,0,t,0,0,0,1),this}equals(e){const t=this.elements,n=e.elements;for(let s=0;s<9;s++)if(t[s]!==n[s])return!1;return!0}fromArray(e,t=0){for(let n=0;n<9;n++)this.elements[n]=e[n+t];return this}toArray(e=[],t=0){const n=this.elements;return e[t]=n[0],e[t+1]=n[1],e[t+2]=n[2],e[t+3]=n[3],e[t+4]=n[4],e[t+5]=n[5],e[t+6]=n[6],e[t+7]=n[7],e[t+8]=n[8],e}clone(){return new this.constructor().fromArray(this.elements)}}const Lr=new Ze;function Lc(i){for(let e=i.length-1;e>=0;--e)if(i[e]>=65535)return!0;return!1}function ds(i){return document.createElementNS("http://www.w3.org/1999/xhtml",i)}function af(){const i=ds("canvas");return i.style.display="block",i}const ll={};function Ni(i){i in ll||(ll[i]=!0,console.warn(i))}function of(i,e,t){return new Promise(function(n,s){function r(){switch(i.clientWaitSync(e,i.SYNC_FLUSH_COMMANDS_BIT,0)){case i.WAIT_FAILED:s();break;case i.TIMEOUT_EXPIRED:setTimeout(r,t);break;default:n()}}setTimeout(r,t)})}const cl=new Ze().set(.4123908,.3575843,.1804808,.212639,.7151687,.0721923,.0193308,.1191948,.9505322),ul=new Ze().set(3.2409699,-1.5373832,-.4986108,-.9692436,1.8759675,.0415551,.0556301,-.203977,1.0569715);function lf(){const i={enabled:!0,workingColorSpace:Hi,spaces:{},convert:function(s,r,a){return this.enabled===!1||r===a||!r||!a||(this.spaces[r].transfer===mt&&(s.r=In(s.r),s.g=In(s.g),s.b=In(s.b)),this.spaces[r].primaries!==this.spaces[a].primaries&&(s.applyMatrix3(this.spaces[r].toXYZ),s.applyMatrix3(this.spaces[a].fromXYZ)),this.spaces[a].transfer===mt&&(s.r=Fi(s.r),s.g=Fi(s.g),s.b=Fi(s.b))),s},workingToColorSpace:function(s,r){return this.convert(s,this.workingColorSpace,r)},colorSpaceToWorking:function(s,r){return this.convert(s,r,this.workingColorSpace)},getPrimaries:function(s){return this.spaces[s].primaries},getTransfer:function(s){return s===Hn?rr:this.spaces[s].transfer},getLuminanceCoefficients:function(s,r=this.workingColorSpace){return s.fromArray(this.spaces[r].luminanceCoefficients)},define:function(s){Object.assign(this.spaces,s)},_getMatrix:function(s,r,a){return s.copy(this.spaces[r].toXYZ).multiply(this.spaces[a].fromXYZ)},_getDrawingBufferColorSpace:function(s){return this.spaces[s].outputColorSpaceConfig.drawingBufferColorSpace},_getUnpackColorSpace:function(s=this.workingColorSpace){return this.spaces[s].workingColorSpaceConfig.unpackColorSpace},fromWorkingColorSpace:function(s,r){return Ni("THREE.ColorManagement: .fromWorkingColorSpace() has been renamed to .workingToColorSpace()."),i.workingToColorSpace(s,r)},toWorkingColorSpace:function(s,r){return Ni("THREE.ColorManagement: .toWorkingColorSpace() has been renamed to .colorSpaceToWorking()."),i.colorSpaceToWorking(s,r)}},e=[.64,.33,.3,.6,.15,.06],t=[.2126,.7152,.0722],n=[.3127,.329];return i.define({[Hi]:{primaries:e,whitePoint:n,transfer:rr,toXYZ:cl,fromXYZ:ul,luminanceCoefficients:t,workingColorSpaceConfig:{unpackColorSpace:rn},outputColorSpaceConfig:{drawingBufferColorSpace:rn}},[rn]:{primaries:e,whitePoint:n,transfer:mt,toXYZ:cl,fromXYZ:ul,luminanceCoefficients:t,outputColorSpaceConfig:{drawingBufferColorSpace:rn}}}),i}const lt=lf();function In(i){return i<.04045?i*.0773993808:Math.pow(i*.9478672986+.0521327014,2.4)}function Fi(i){return i<.0031308?i*12.92:1.055*Math.pow(i,.41666)-.055}let vi;class cf{static getDataURL(e,t="image/png"){if(/^data:/i.test(e.src)||typeof HTMLCanvasElement>"u")return e.src;let n;if(e instanceof HTMLCanvasElement)n=e;else{vi===void 0&&(vi=ds("canvas")),vi.width=e.width,vi.height=e.height;const s=vi.getContext("2d");e instanceof ImageData?s.putImageData(e,0,0):s.drawImage(e,0,0,e.width,e.height),n=vi}return n.toDataURL(t)}static sRGBToLinear(e){if(typeof HTMLImageElement<"u"&&e instanceof HTMLImageElement||typeof HTMLCanvasElement<"u"&&e instanceof HTMLCanvasElement||typeof ImageBitmap<"u"&&e instanceof ImageBitmap){const t=ds("canvas");t.width=e.width,t.height=e.height;const n=t.getContext("2d");n.drawImage(e,0,0,e.width,e.height);const s=n.getImageData(0,0,e.width,e.height),r=s.data;for(let a=0;a<r.length;a++)r[a]=In(r[a]/255)*255;return n.putImageData(s,0,0),t}else if(e.data){const t=e.data.slice(0);for(let n=0;n<t.length;n++)t instanceof Uint8Array||t instanceof Uint8ClampedArray?t[n]=Math.floor(In(t[n]/255)*255):t[n]=In(t[n]);return{data:t,width:e.width,height:e.height}}else return console.warn("THREE.ImageUtils.sRGBToLinear(): Unsupported image type. No color space conversion applied."),e}}let uf=0;class wo{constructor(e=null){this.isSource=!0,Object.defineProperty(this,"id",{value:uf++}),this.uuid=$i(),this.data=e,this.dataReady=!0,this.version=0}getSize(e){const t=this.data;return t instanceof HTMLVideoElement?e.set(t.videoWidth,t.videoHeight,0):t instanceof VideoFrame?e.set(t.displayHeight,t.displayWidth,0):t!==null?e.set(t.width,t.height,t.depth||0):e.set(0,0,0),e}set needsUpdate(e){e===!0&&this.version++}toJSON(e){const t=e===void 0||typeof e=="string";if(!t&&e.images[this.uuid]!==void 0)return e.images[this.uuid];const n={uuid:this.uuid,url:""},s=this.data;if(s!==null){let r;if(Array.isArray(s)){r=[];for(let a=0,o=s.length;a<o;a++)s[a].isDataTexture?r.push(Dr(s[a].image)):r.push(Dr(s[a]))}else r=Dr(s);n.url=r}return t||(e.images[this.uuid]=n),n}}function Dr(i){return typeof HTMLImageElement<"u"&&i instanceof HTMLImageElement||typeof HTMLCanvasElement<"u"&&i instanceof HTMLCanvasElement||typeof ImageBitmap<"u"&&i instanceof ImageBitmap?cf.getDataURL(i):i.data?{data:Array.from(i.data),width:i.width,height:i.height,type:i.data.constructor.name}:(console.warn("THREE.Texture: Unable to serialize Texture."),{})}let df=0;const Ir=new O;class Bt extends Wi{constructor(e=Bt.DEFAULT_IMAGE,t=Bt.DEFAULT_MAPPING,n=Cn,s=Cn,r=an,a=li,o=hn,l=Mn,c=Bt.DEFAULT_ANISOTROPY,u=Hn){super(),this.isTexture=!0,Object.defineProperty(this,"id",{value:df++}),this.uuid=$i(),this.name="",this.source=new wo(e),this.mipmaps=[],this.mapping=t,this.channel=0,this.wrapS=n,this.wrapT=s,this.magFilter=r,this.minFilter=a,this.anisotropy=c,this.format=o,this.internalFormat=null,this.type=l,this.offset=new et(0,0),this.repeat=new et(1,1),this.center=new et(0,0),this.rotation=0,this.matrixAutoUpdate=!0,this.matrix=new Ze,this.generateMipmaps=!0,this.premultiplyAlpha=!1,this.flipY=!0,this.unpackAlignment=4,this.colorSpace=u,this.userData={},this.updateRanges=[],this.version=0,this.onUpdate=null,this.renderTarget=null,this.isRenderTargetTexture=!1,this.isArrayTexture=!!(e&&e.depth&&e.depth>1),this.pmremVersion=0}get width(){return this.source.getSize(Ir).x}get height(){return this.source.getSize(Ir).y}get depth(){return this.source.getSize(Ir).z}get image(){return this.source.data}set image(e=null){this.source.data=e}updateMatrix(){this.matrix.setUvTransform(this.offset.x,this.offset.y,this.repeat.x,this.repeat.y,this.rotation,this.center.x,this.center.y)}addUpdateRange(e,t){this.updateRanges.push({start:e,count:t})}clearUpdateRanges(){this.updateRanges.length=0}clone(){return new this.constructor().copy(this)}copy(e){return this.name=e.name,this.source=e.source,this.mipmaps=e.mipmaps.slice(0),this.mapping=e.mapping,this.channel=e.channel,this.wrapS=e.wrapS,this.wrapT=e.wrapT,this.magFilter=e.magFilter,this.minFilter=e.minFilter,this.anisotropy=e.anisotropy,this.format=e.format,this.internalFormat=e.internalFormat,this.type=e.type,this.offset.copy(e.offset),this.repeat.copy(e.repeat),this.center.copy(e.center),this.rotation=e.rotation,this.matrixAutoUpdate=e.matrixAutoUpdate,this.matrix.copy(e.matrix),this.generateMipmaps=e.generateMipmaps,this.premultiplyAlpha=e.premultiplyAlpha,this.flipY=e.flipY,this.unpackAlignment=e.unpackAlignment,this.colorSpace=e.colorSpace,this.renderTarget=e.renderTarget,this.isRenderTargetTexture=e.isRenderTargetTexture,this.isArrayTexture=e.isArrayTexture,this.userData=JSON.parse(JSON.stringify(e.userData)),this.needsUpdate=!0,this}setValues(e){for(const t in e){const n=e[t];if(n===void 0){console.warn(`THREE.Texture.setValues(): parameter '${t}' has value of undefined.`);continue}const s=this[t];if(s===void 0){console.warn(`THREE.Texture.setValues(): property '${t}' does not exist.`);continue}s&&n&&s.isVector2&&n.isVector2||s&&n&&s.isVector3&&n.isVector3||s&&n&&s.isMatrix3&&n.isMatrix3?s.copy(n):this[t]=n}}toJSON(e){const t=e===void 0||typeof e=="string";if(!t&&e.textures[this.uuid]!==void 0)return e.textures[this.uuid];const n={metadata:{version:4.7,type:"Texture",generator:"Texture.toJSON"},uuid:this.uuid,name:this.name,image:this.source.toJSON(e).uuid,mapping:this.mapping,channel:this.channel,repeat:[this.repeat.x,this.repeat.y],offset:[this.offset.x,this.offset.y],center:[this.center.x,this.center.y],rotation:this.rotation,wrap:[this.wrapS,this.wrapT],format:this.format,internalFormat:this.internalFormat,type:this.type,colorSpace:this.colorSpace,minFilter:this.minFilter,magFilter:this.magFilter,anisotropy:this.anisotropy,flipY:this.flipY,generateMipmaps:this.generateMipmaps,premultiplyAlpha:this.premultiplyAlpha,unpackAlignment:this.unpackAlignment};return Object.keys(this.userData).length>0&&(n.userData=this.userData),t||(e.textures[this.uuid]=n),n}dispose(){this.dispatchEvent({type:"dispose"})}transformUv(e){if(this.mapping!==Mc)return e;if(e.applyMatrix3(this.matrix),e.x<0||e.x>1)switch(this.wrapS){case Ca:e.x=e.x-Math.floor(e.x);break;case Cn:e.x=e.x<0?0:1;break;case Pa:Math.abs(Math.floor(e.x)%2)===1?e.x=Math.ceil(e.x)-e.x:e.x=e.x-Math.floor(e.x);break}if(e.y<0||e.y>1)switch(this.wrapT){case Ca:e.y=e.y-Math.floor(e.y);break;case Cn:e.y=e.y<0?0:1;break;case Pa:Math.abs(Math.floor(e.y)%2)===1?e.y=Math.ceil(e.y)-e.y:e.y=e.y-Math.floor(e.y);break}return this.flipY&&(e.y=1-e.y),e}set needsUpdate(e){e===!0&&(this.version++,this.source.needsUpdate=!0)}set needsPMREMUpdate(e){e===!0&&this.pmremVersion++}}Bt.DEFAULT_IMAGE=null;Bt.DEFAULT_MAPPING=Mc;Bt.DEFAULT_ANISOTROPY=1;class At{constructor(e=0,t=0,n=0,s=1){At.prototype.isVector4=!0,this.x=e,this.y=t,this.z=n,this.w=s}get width(){return this.z}set width(e){this.z=e}get height(){return this.w}set height(e){this.w=e}set(e,t,n,s){return this.x=e,this.y=t,this.z=n,this.w=s,this}setScalar(e){return this.x=e,this.y=e,this.z=e,this.w=e,this}setX(e){return this.x=e,this}setY(e){return this.y=e,this}setZ(e){return this.z=e,this}setW(e){return this.w=e,this}setComponent(e,t){switch(e){case 0:this.x=t;break;case 1:this.y=t;break;case 2:this.z=t;break;case 3:this.w=t;break;default:throw new Error("index is out of range: "+e)}return this}getComponent(e){switch(e){case 0:return this.x;case 1:return this.y;case 2:return this.z;case 3:return this.w;default:throw new Error("index is out of range: "+e)}}clone(){return new this.constructor(this.x,this.y,this.z,this.w)}copy(e){return this.x=e.x,this.y=e.y,this.z=e.z,this.w=e.w!==void 0?e.w:1,this}add(e){return this.x+=e.x,this.y+=e.y,this.z+=e.z,this.w+=e.w,this}addScalar(e){return this.x+=e,this.y+=e,this.z+=e,this.w+=e,this}addVectors(e,t){return this.x=e.x+t.x,this.y=e.y+t.y,this.z=e.z+t.z,this.w=e.w+t.w,this}addScaledVector(e,t){return this.x+=e.x*t,this.y+=e.y*t,this.z+=e.z*t,this.w+=e.w*t,this}sub(e){return this.x-=e.x,this.y-=e.y,this.z-=e.z,this.w-=e.w,this}subScalar(e){return this.x-=e,this.y-=e,this.z-=e,this.w-=e,this}subVectors(e,t){return this.x=e.x-t.x,this.y=e.y-t.y,this.z=e.z-t.z,this.w=e.w-t.w,this}multiply(e){return this.x*=e.x,this.y*=e.y,this.z*=e.z,this.w*=e.w,this}multiplyScalar(e){return this.x*=e,this.y*=e,this.z*=e,this.w*=e,this}applyMatrix4(e){const t=this.x,n=this.y,s=this.z,r=this.w,a=e.elements;return this.x=a[0]*t+a[4]*n+a[8]*s+a[12]*r,this.y=a[1]*t+a[5]*n+a[9]*s+a[13]*r,this.z=a[2]*t+a[6]*n+a[10]*s+a[14]*r,this.w=a[3]*t+a[7]*n+a[11]*s+a[15]*r,this}divide(e){return this.x/=e.x,this.y/=e.y,this.z/=e.z,this.w/=e.w,this}divideScalar(e){return this.multiplyScalar(1/e)}setAxisAngleFromQuaternion(e){this.w=2*Math.acos(e.w);const t=Math.sqrt(1-e.w*e.w);return t<1e-4?(this.x=1,this.y=0,this.z=0):(this.x=e.x/t,this.y=e.y/t,this.z=e.z/t),this}setAxisAngleFromRotationMatrix(e){let t,n,s,r;const l=e.elements,c=l[0],u=l[4],d=l[8],h=l[1],p=l[5],g=l[9],_=l[2],m=l[6],f=l[10];if(Math.abs(u-h)<.01&&Math.abs(d-_)<.01&&Math.abs(g-m)<.01){if(Math.abs(u+h)<.1&&Math.abs(d+_)<.1&&Math.abs(g+m)<.1&&Math.abs(c+p+f-3)<.1)return this.set(1,0,0,0),this;t=Math.PI;const b=(c+1)/2,v=(p+1)/2,T=(f+1)/2,A=(u+h)/4,C=(d+_)/4,I=(g+m)/4;return b>v&&b>T?b<.01?(n=0,s=.707106781,r=.707106781):(n=Math.sqrt(b),s=A/n,r=C/n):v>T?v<.01?(n=.707106781,s=0,r=.707106781):(s=Math.sqrt(v),n=A/s,r=I/s):T<.01?(n=.707106781,s=.707106781,r=0):(r=Math.sqrt(T),n=C/r,s=I/r),this.set(n,s,r,t),this}let E=Math.sqrt((m-g)*(m-g)+(d-_)*(d-_)+(h-u)*(h-u));return Math.abs(E)<.001&&(E=1),this.x=(m-g)/E,this.y=(d-_)/E,this.z=(h-u)/E,this.w=Math.acos((c+p+f-1)/2),this}setFromMatrixPosition(e){const t=e.elements;return this.x=t[12],this.y=t[13],this.z=t[14],this.w=t[15],this}min(e){return this.x=Math.min(this.x,e.x),this.y=Math.min(this.y,e.y),this.z=Math.min(this.z,e.z),this.w=Math.min(this.w,e.w),this}max(e){return this.x=Math.max(this.x,e.x),this.y=Math.max(this.y,e.y),this.z=Math.max(this.z,e.z),this.w=Math.max(this.w,e.w),this}clamp(e,t){return this.x=it(this.x,e.x,t.x),this.y=it(this.y,e.y,t.y),this.z=it(this.z,e.z,t.z),this.w=it(this.w,e.w,t.w),this}clampScalar(e,t){return this.x=it(this.x,e,t),this.y=it(this.y,e,t),this.z=it(this.z,e,t),this.w=it(this.w,e,t),this}clampLength(e,t){const n=this.length();return this.divideScalar(n||1).multiplyScalar(it(n,e,t))}floor(){return this.x=Math.floor(this.x),this.y=Math.floor(this.y),this.z=Math.floor(this.z),this.w=Math.floor(this.w),this}ceil(){return this.x=Math.ceil(this.x),this.y=Math.ceil(this.y),this.z=Math.ceil(this.z),this.w=Math.ceil(this.w),this}round(){return this.x=Math.round(this.x),this.y=Math.round(this.y),this.z=Math.round(this.z),this.w=Math.round(this.w),this}roundToZero(){return this.x=Math.trunc(this.x),this.y=Math.trunc(this.y),this.z=Math.trunc(this.z),this.w=Math.trunc(this.w),this}negate(){return this.x=-this.x,this.y=-this.y,this.z=-this.z,this.w=-this.w,this}dot(e){return this.x*e.x+this.y*e.y+this.z*e.z+this.w*e.w}lengthSq(){return this.x*this.x+this.y*this.y+this.z*this.z+this.w*this.w}length(){return Math.sqrt(this.x*this.x+this.y*this.y+this.z*this.z+this.w*this.w)}manhattanLength(){return Math.abs(this.x)+Math.abs(this.y)+Math.abs(this.z)+Math.abs(this.w)}normalize(){return this.divideScalar(this.length()||1)}setLength(e){return this.normalize().multiplyScalar(e)}lerp(e,t){return this.x+=(e.x-this.x)*t,this.y+=(e.y-this.y)*t,this.z+=(e.z-this.z)*t,this.w+=(e.w-this.w)*t,this}lerpVectors(e,t,n){return this.x=e.x+(t.x-e.x)*n,this.y=e.y+(t.y-e.y)*n,this.z=e.z+(t.z-e.z)*n,this.w=e.w+(t.w-e.w)*n,this}equals(e){return e.x===this.x&&e.y===this.y&&e.z===this.z&&e.w===this.w}fromArray(e,t=0){return this.x=e[t],this.y=e[t+1],this.z=e[t+2],this.w=e[t+3],this}toArray(e=[],t=0){return e[t]=this.x,e[t+1]=this.y,e[t+2]=this.z,e[t+3]=this.w,e}fromBufferAttribute(e,t){return this.x=e.getX(t),this.y=e.getY(t),this.z=e.getZ(t),this.w=e.getW(t),this}random(){return this.x=Math.random(),this.y=Math.random(),this.z=Math.random(),this.w=Math.random(),this}*[Symbol.iterator](){yield this.x,yield this.y,yield this.z,yield this.w}}class hf extends Wi{constructor(e=1,t=1,n={}){super(),n=Object.assign({generateMipmaps:!1,internalFormat:null,minFilter:an,depthBuffer:!0,stencilBuffer:!1,resolveDepthBuffer:!0,resolveStencilBuffer:!0,depthTexture:null,samples:0,count:1,depth:1,multiview:!1},n),this.isRenderTarget=!0,this.width=e,this.height=t,this.depth=n.depth,this.scissor=new At(0,0,e,t),this.scissorTest=!1,this.viewport=new At(0,0,e,t);const s={width:e,height:t,depth:n.depth},r=new Bt(s);this.textures=[];const a=n.count;for(let o=0;o<a;o++)this.textures[o]=r.clone(),this.textures[o].isRenderTargetTexture=!0,this.textures[o].renderTarget=this;this._setTextureOptions(n),this.depthBuffer=n.depthBuffer,this.stencilBuffer=n.stencilBuffer,this.resolveDepthBuffer=n.resolveDepthBuffer,this.resolveStencilBuffer=n.resolveStencilBuffer,this._depthTexture=null,this.depthTexture=n.depthTexture,this.samples=n.samples,this.multiview=n.multiview}_setTextureOptions(e={}){const t={minFilter:an,generateMipmaps:!1,flipY:!1,internalFormat:null};e.mapping!==void 0&&(t.mapping=e.mapping),e.wrapS!==void 0&&(t.wrapS=e.wrapS),e.wrapT!==void 0&&(t.wrapT=e.wrapT),e.wrapR!==void 0&&(t.wrapR=e.wrapR),e.magFilter!==void 0&&(t.magFilter=e.magFilter),e.minFilter!==void 0&&(t.minFilter=e.minFilter),e.format!==void 0&&(t.format=e.format),e.type!==void 0&&(t.type=e.type),e.anisotropy!==void 0&&(t.anisotropy=e.anisotropy),e.colorSpace!==void 0&&(t.colorSpace=e.colorSpace),e.flipY!==void 0&&(t.flipY=e.flipY),e.generateMipmaps!==void 0&&(t.generateMipmaps=e.generateMipmaps),e.internalFormat!==void 0&&(t.internalFormat=e.internalFormat);for(let n=0;n<this.textures.length;n++)this.textures[n].setValues(t)}get texture(){return this.textures[0]}set texture(e){this.textures[0]=e}set depthTexture(e){this._depthTexture!==null&&(this._depthTexture.renderTarget=null),e!==null&&(e.renderTarget=this),this._depthTexture=e}get depthTexture(){return this._depthTexture}setSize(e,t,n=1){if(this.width!==e||this.height!==t||this.depth!==n){this.width=e,this.height=t,this.depth=n;for(let s=0,r=this.textures.length;s<r;s++)this.textures[s].image.width=e,this.textures[s].image.height=t,this.textures[s].image.depth=n,this.textures[s].isArrayTexture=this.textures[s].image.depth>1;this.dispose()}this.viewport.set(0,0,e,t),this.scissor.set(0,0,e,t)}clone(){return new this.constructor().copy(this)}copy(e){this.width=e.width,this.height=e.height,this.depth=e.depth,this.scissor.copy(e.scissor),this.scissorTest=e.scissorTest,this.viewport.copy(e.viewport),this.textures.length=0;for(let t=0,n=e.textures.length;t<n;t++){this.textures[t]=e.textures[t].clone(),this.textures[t].isRenderTargetTexture=!0,this.textures[t].renderTarget=this;const s=Object.assign({},e.textures[t].image);this.textures[t].source=new wo(s)}return this.depthBuffer=e.depthBuffer,this.stencilBuffer=e.stencilBuffer,this.resolveDepthBuffer=e.resolveDepthBuffer,this.resolveStencilBuffer=e.resolveStencilBuffer,e.depthTexture!==null&&(this.depthTexture=e.depthTexture.clone()),this.samples=e.samples,this}dispose(){this.dispatchEvent({type:"dispose"})}}class di extends hf{constructor(e=1,t=1,n={}){super(e,t,n),this.isWebGLRenderTarget=!0}}class Dc extends Bt{constructor(e=null,t=1,n=1,s=1){super(null),this.isDataArrayTexture=!0,this.image={data:e,width:t,height:n,depth:s},this.magFilter=fn,this.minFilter=fn,this.wrapR=Cn,this.generateMipmaps=!1,this.flipY=!1,this.unpackAlignment=1,this.layerUpdates=new Set}addLayerUpdate(e){this.layerUpdates.add(e)}clearLayerUpdates(){this.layerUpdates.clear()}}class ff extends Bt{constructor(e=null,t=1,n=1,s=1){super(null),this.isData3DTexture=!0,this.image={data:e,width:t,height:n,depth:s},this.magFilter=fn,this.minFilter=fn,this.wrapR=Cn,this.generateMipmaps=!1,this.flipY=!1,this.unpackAlignment=1}}class gs{constructor(e=new O(1/0,1/0,1/0),t=new O(-1/0,-1/0,-1/0)){this.isBox3=!0,this.min=e,this.max=t}set(e,t){return this.min.copy(e),this.max.copy(t),this}setFromArray(e){this.makeEmpty();for(let t=0,n=e.length;t<n;t+=3)this.expandByPoint(ln.fromArray(e,t));return this}setFromBufferAttribute(e){this.makeEmpty();for(let t=0,n=e.count;t<n;t++)this.expandByPoint(ln.fromBufferAttribute(e,t));return this}setFromPoints(e){this.makeEmpty();for(let t=0,n=e.length;t<n;t++)this.expandByPoint(e[t]);return this}setFromCenterAndSize(e,t){const n=ln.copy(t).multiplyScalar(.5);return this.min.copy(e).sub(n),this.max.copy(e).add(n),this}setFromObject(e,t=!1){return this.makeEmpty(),this.expandByObject(e,t)}clone(){return new this.constructor().copy(this)}copy(e){return this.min.copy(e.min),this.max.copy(e.max),this}makeEmpty(){return this.min.x=this.min.y=this.min.z=1/0,this.max.x=this.max.y=this.max.z=-1/0,this}isEmpty(){return this.max.x<this.min.x||this.max.y<this.min.y||this.max.z<this.min.z}getCenter(e){return this.isEmpty()?e.set(0,0,0):e.addVectors(this.min,this.max).multiplyScalar(.5)}getSize(e){return this.isEmpty()?e.set(0,0,0):e.subVectors(this.max,this.min)}expandByPoint(e){return this.min.min(e),this.max.max(e),this}expandByVector(e){return this.min.sub(e),this.max.add(e),this}expandByScalar(e){return this.min.addScalar(-e),this.max.addScalar(e),this}expandByObject(e,t=!1){e.updateWorldMatrix(!1,!1);const n=e.geometry;if(n!==void 0){const r=n.getAttribute("position");if(t===!0&&r!==void 0&&e.isInstancedMesh!==!0)for(let a=0,o=r.count;a<o;a++)e.isMesh===!0?e.getVertexPosition(a,ln):ln.fromBufferAttribute(r,a),ln.applyMatrix4(e.matrixWorld),this.expandByPoint(ln);else e.boundingBox!==void 0?(e.boundingBox===null&&e.computeBoundingBox(),bs.copy(e.boundingBox)):(n.boundingBox===null&&n.computeBoundingBox(),bs.copy(n.boundingBox)),bs.applyMatrix4(e.matrixWorld),this.union(bs)}const s=e.children;for(let r=0,a=s.length;r<a;r++)this.expandByObject(s[r],t);return this}containsPoint(e){return e.x>=this.min.x&&e.x<=this.max.x&&e.y>=this.min.y&&e.y<=this.max.y&&e.z>=this.min.z&&e.z<=this.max.z}containsBox(e){return this.min.x<=e.min.x&&e.max.x<=this.max.x&&this.min.y<=e.min.y&&e.max.y<=this.max.y&&this.min.z<=e.min.z&&e.max.z<=this.max.z}getParameter(e,t){return t.set((e.x-this.min.x)/(this.max.x-this.min.x),(e.y-this.min.y)/(this.max.y-this.min.y),(e.z-this.min.z)/(this.max.z-this.min.z))}intersectsBox(e){return e.max.x>=this.min.x&&e.min.x<=this.max.x&&e.max.y>=this.min.y&&e.min.y<=this.max.y&&e.max.z>=this.min.z&&e.min.z<=this.max.z}intersectsSphere(e){return this.clampPoint(e.center,ln),ln.distanceToSquared(e.center)<=e.radius*e.radius}intersectsPlane(e){let t,n;return e.normal.x>0?(t=e.normal.x*this.min.x,n=e.normal.x*this.max.x):(t=e.normal.x*this.max.x,n=e.normal.x*this.min.x),e.normal.y>0?(t+=e.normal.y*this.min.y,n+=e.normal.y*this.max.y):(t+=e.normal.y*this.max.y,n+=e.normal.y*this.min.y),e.normal.z>0?(t+=e.normal.z*this.min.z,n+=e.normal.z*this.max.z):(t+=e.normal.z*this.max.z,n+=e.normal.z*this.min.z),t<=-e.constant&&n>=-e.constant}intersectsTriangle(e){if(this.isEmpty())return!1;this.getCenter(Yi),Es.subVectors(this.max,Yi),xi.subVectors(e.a,Yi),Mi.subVectors(e.b,Yi),Si.subVectors(e.c,Yi),Nn.subVectors(Mi,xi),Fn.subVectors(Si,Mi),Jn.subVectors(xi,Si);let t=[0,-Nn.z,Nn.y,0,-Fn.z,Fn.y,0,-Jn.z,Jn.y,Nn.z,0,-Nn.x,Fn.z,0,-Fn.x,Jn.z,0,-Jn.x,-Nn.y,Nn.x,0,-Fn.y,Fn.x,0,-Jn.y,Jn.x,0];return!Ur(t,xi,Mi,Si,Es)||(t=[1,0,0,0,1,0,0,0,1],!Ur(t,xi,Mi,Si,Es))?!1:(Ts.crossVectors(Nn,Fn),t=[Ts.x,Ts.y,Ts.z],Ur(t,xi,Mi,Si,Es))}clampPoint(e,t){return t.copy(e).clamp(this.min,this.max)}distanceToPoint(e){return this.clampPoint(e,ln).distanceTo(e)}getBoundingSphere(e){return this.isEmpty()?e.makeEmpty():(this.getCenter(e.center),e.radius=this.getSize(ln).length()*.5),e}intersect(e){return this.min.max(e.min),this.max.min(e.max),this.isEmpty()&&this.makeEmpty(),this}union(e){return this.min.min(e.min),this.max.max(e.max),this}applyMatrix4(e){return this.isEmpty()?this:(bn[0].set(this.min.x,this.min.y,this.min.z).applyMatrix4(e),bn[1].set(this.min.x,this.min.y,this.max.z).applyMatrix4(e),bn[2].set(this.min.x,this.max.y,this.min.z).applyMatrix4(e),bn[3].set(this.min.x,this.max.y,this.max.z).applyMatrix4(e),bn[4].set(this.max.x,this.min.y,this.min.z).applyMatrix4(e),bn[5].set(this.max.x,this.min.y,this.max.z).applyMatrix4(e),bn[6].set(this.max.x,this.max.y,this.min.z).applyMatrix4(e),bn[7].set(this.max.x,this.max.y,this.max.z).applyMatrix4(e),this.setFromPoints(bn),this)}translate(e){return this.min.add(e),this.max.add(e),this}equals(e){return e.min.equals(this.min)&&e.max.equals(this.max)}toJSON(){return{min:this.min.toArray(),max:this.max.toArray()}}fromJSON(e){return this.min.fromArray(e.min),this.max.fromArray(e.max),this}}const bn=[new O,new O,new O,new O,new O,new O,new O,new O],ln=new O,bs=new gs,xi=new O,Mi=new O,Si=new O,Nn=new O,Fn=new O,Jn=new O,Yi=new O,Es=new O,Ts=new O,Qn=new O;function Ur(i,e,t,n,s){for(let r=0,a=i.length-3;r<=a;r+=3){Qn.fromArray(i,r);const o=s.x*Math.abs(Qn.x)+s.y*Math.abs(Qn.y)+s.z*Math.abs(Qn.z),l=e.dot(Qn),c=t.dot(Qn),u=n.dot(Qn);if(Math.max(-Math.max(l,c,u),Math.min(l,c,u))>o)return!1}return!0}const pf=new gs,ji=new O,kr=new O;class _s{constructor(e=new O,t=-1){this.isSphere=!0,this.center=e,this.radius=t}set(e,t){return this.center.copy(e),this.radius=t,this}setFromPoints(e,t){const n=this.center;t!==void 0?n.copy(t):pf.setFromPoints(e).getCenter(n);let s=0;for(let r=0,a=e.length;r<a;r++)s=Math.max(s,n.distanceToSquared(e[r]));return this.radius=Math.sqrt(s),this}copy(e){return this.center.copy(e.center),this.radius=e.radius,this}isEmpty(){return this.radius<0}makeEmpty(){return this.center.set(0,0,0),this.radius=-1,this}containsPoint(e){return e.distanceToSquared(this.center)<=this.radius*this.radius}distanceToPoint(e){return e.distanceTo(this.center)-this.radius}intersectsSphere(e){const t=this.radius+e.radius;return e.center.distanceToSquared(this.center)<=t*t}intersectsBox(e){return e.intersectsSphere(this)}intersectsPlane(e){return Math.abs(e.distanceToPoint(this.center))<=this.radius}clampPoint(e,t){const n=this.center.distanceToSquared(e);return t.copy(e),n>this.radius*this.radius&&(t.sub(this.center).normalize(),t.multiplyScalar(this.radius).add(this.center)),t}getBoundingBox(e){return this.isEmpty()?(e.makeEmpty(),e):(e.set(this.center,this.center),e.expandByScalar(this.radius),e)}applyMatrix4(e){return this.center.applyMatrix4(e),this.radius=this.radius*e.getMaxScaleOnAxis(),this}translate(e){return this.center.add(e),this}expandByPoint(e){if(this.isEmpty())return this.center.copy(e),this.radius=0,this;ji.subVectors(e,this.center);const t=ji.lengthSq();if(t>this.radius*this.radius){const n=Math.sqrt(t),s=(n-this.radius)*.5;this.center.addScaledVector(ji,s/n),this.radius+=s}return this}union(e){return e.isEmpty()?this:this.isEmpty()?(this.copy(e),this):(this.center.equals(e.center)===!0?this.radius=Math.max(this.radius,e.radius):(kr.subVectors(e.center,this.center).setLength(e.radius),this.expandByPoint(ji.copy(e.center).add(kr)),this.expandByPoint(ji.copy(e.center).sub(kr))),this)}equals(e){return e.center.equals(this.center)&&e.radius===this.radius}clone(){return new this.constructor().copy(this)}toJSON(){return{radius:this.radius,center:this.center.toArray()}}fromJSON(e){return this.radius=e.radius,this.center.fromArray(e.center),this}}const En=new O,Nr=new O,ws=new O,On=new O,Fr=new O,As=new O,Or=new O;class vr{constructor(e=new O,t=new O(0,0,-1)){this.origin=e,this.direction=t}set(e,t){return this.origin.copy(e),this.direction.copy(t),this}copy(e){return this.origin.copy(e.origin),this.direction.copy(e.direction),this}at(e,t){return t.copy(this.origin).addScaledVector(this.direction,e)}lookAt(e){return this.direction.copy(e).sub(this.origin).normalize(),this}recast(e){return this.origin.copy(this.at(e,En)),this}closestPointToPoint(e,t){t.subVectors(e,this.origin);const n=t.dot(this.direction);return n<0?t.copy(this.origin):t.copy(this.origin).addScaledVector(this.direction,n)}distanceToPoint(e){return Math.sqrt(this.distanceSqToPoint(e))}distanceSqToPoint(e){const t=En.subVectors(e,this.origin).dot(this.direction);return t<0?this.origin.distanceToSquared(e):(En.copy(this.origin).addScaledVector(this.direction,t),En.distanceToSquared(e))}distanceSqToSegment(e,t,n,s){Nr.copy(e).add(t).multiplyScalar(.5),ws.copy(t).sub(e).normalize(),On.copy(this.origin).sub(Nr);const r=e.distanceTo(t)*.5,a=-this.direction.dot(ws),o=On.dot(this.direction),l=-On.dot(ws),c=On.lengthSq(),u=Math.abs(1-a*a);let d,h,p,g;if(u>0)if(d=a*l-o,h=a*o-l,g=r*u,d>=0)if(h>=-g)if(h<=g){const _=1/u;d*=_,h*=_,p=d*(d+a*h+2*o)+h*(a*d+h+2*l)+c}else h=r,d=Math.max(0,-(a*h+o)),p=-d*d+h*(h+2*l)+c;else h=-r,d=Math.max(0,-(a*h+o)),p=-d*d+h*(h+2*l)+c;else h<=-g?(d=Math.max(0,-(-a*r+o)),h=d>0?-r:Math.min(Math.max(-r,-l),r),p=-d*d+h*(h+2*l)+c):h<=g?(d=0,h=Math.min(Math.max(-r,-l),r),p=h*(h+2*l)+c):(d=Math.max(0,-(a*r+o)),h=d>0?r:Math.min(Math.max(-r,-l),r),p=-d*d+h*(h+2*l)+c);else h=a>0?-r:r,d=Math.max(0,-(a*h+o)),p=-d*d+h*(h+2*l)+c;return n&&n.copy(this.origin).addScaledVector(this.direction,d),s&&s.copy(Nr).addScaledVector(ws,h),p}intersectSphere(e,t){En.subVectors(e.center,this.origin);const n=En.dot(this.direction),s=En.dot(En)-n*n,r=e.radius*e.radius;if(s>r)return null;const a=Math.sqrt(r-s),o=n-a,l=n+a;return l<0?null:o<0?this.at(l,t):this.at(o,t)}intersectsSphere(e){return e.radius<0?!1:this.distanceSqToPoint(e.center)<=e.radius*e.radius}distanceToPlane(e){const t=e.normal.dot(this.direction);if(t===0)return e.distanceToPoint(this.origin)===0?0:null;const n=-(this.origin.dot(e.normal)+e.constant)/t;return n>=0?n:null}intersectPlane(e,t){const n=this.distanceToPlane(e);return n===null?null:this.at(n,t)}intersectsPlane(e){const t=e.distanceToPoint(this.origin);return t===0||e.normal.dot(this.direction)*t<0}intersectBox(e,t){let n,s,r,a,o,l;const c=1/this.direction.x,u=1/this.direction.y,d=1/this.direction.z,h=this.origin;return c>=0?(n=(e.min.x-h.x)*c,s=(e.max.x-h.x)*c):(n=(e.max.x-h.x)*c,s=(e.min.x-h.x)*c),u>=0?(r=(e.min.y-h.y)*u,a=(e.max.y-h.y)*u):(r=(e.max.y-h.y)*u,a=(e.min.y-h.y)*u),n>a||r>s||((r>n||isNaN(n))&&(n=r),(a<s||isNaN(s))&&(s=a),d>=0?(o=(e.min.z-h.z)*d,l=(e.max.z-h.z)*d):(o=(e.max.z-h.z)*d,l=(e.min.z-h.z)*d),n>l||o>s)||((o>n||n!==n)&&(n=o),(l<s||s!==s)&&(s=l),s<0)?null:this.at(n>=0?n:s,t)}intersectsBox(e){return this.intersectBox(e,En)!==null}intersectTriangle(e,t,n,s,r){Fr.subVectors(t,e),As.subVectors(n,e),Or.crossVectors(Fr,As);let a=this.direction.dot(Or),o;if(a>0){if(s)return null;o=1}else if(a<0)o=-1,a=-a;else return null;On.subVectors(this.origin,e);const l=o*this.direction.dot(As.crossVectors(On,As));if(l<0)return null;const c=o*this.direction.dot(Fr.cross(On));if(c<0||l+c>a)return null;const u=-o*On.dot(Or);return u<0?null:this.at(u/a,r)}applyMatrix4(e){return this.origin.applyMatrix4(e),this.direction.transformDirection(e),this}equals(e){return e.origin.equals(this.origin)&&e.direction.equals(this.direction)}clone(){return new this.constructor().copy(this)}}class bt{constructor(e,t,n,s,r,a,o,l,c,u,d,h,p,g,_,m){bt.prototype.isMatrix4=!0,this.elements=[1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1],e!==void 0&&this.set(e,t,n,s,r,a,o,l,c,u,d,h,p,g,_,m)}set(e,t,n,s,r,a,o,l,c,u,d,h,p,g,_,m){const f=this.elements;return f[0]=e,f[4]=t,f[8]=n,f[12]=s,f[1]=r,f[5]=a,f[9]=o,f[13]=l,f[2]=c,f[6]=u,f[10]=d,f[14]=h,f[3]=p,f[7]=g,f[11]=_,f[15]=m,this}identity(){return this.set(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1),this}clone(){return new bt().fromArray(this.elements)}copy(e){const t=this.elements,n=e.elements;return t[0]=n[0],t[1]=n[1],t[2]=n[2],t[3]=n[3],t[4]=n[4],t[5]=n[5],t[6]=n[6],t[7]=n[7],t[8]=n[8],t[9]=n[9],t[10]=n[10],t[11]=n[11],t[12]=n[12],t[13]=n[13],t[14]=n[14],t[15]=n[15],this}copyPosition(e){const t=this.elements,n=e.elements;return t[12]=n[12],t[13]=n[13],t[14]=n[14],this}setFromMatrix3(e){const t=e.elements;return this.set(t[0],t[3],t[6],0,t[1],t[4],t[7],0,t[2],t[5],t[8],0,0,0,0,1),this}extractBasis(e,t,n){return e.setFromMatrixColumn(this,0),t.setFromMatrixColumn(this,1),n.setFromMatrixColumn(this,2),this}makeBasis(e,t,n){return this.set(e.x,t.x,n.x,0,e.y,t.y,n.y,0,e.z,t.z,n.z,0,0,0,0,1),this}extractRotation(e){const t=this.elements,n=e.elements,s=1/yi.setFromMatrixColumn(e,0).length(),r=1/yi.setFromMatrixColumn(e,1).length(),a=1/yi.setFromMatrixColumn(e,2).length();return t[0]=n[0]*s,t[1]=n[1]*s,t[2]=n[2]*s,t[3]=0,t[4]=n[4]*r,t[5]=n[5]*r,t[6]=n[6]*r,t[7]=0,t[8]=n[8]*a,t[9]=n[9]*a,t[10]=n[10]*a,t[11]=0,t[12]=0,t[13]=0,t[14]=0,t[15]=1,this}makeRotationFromEuler(e){const t=this.elements,n=e.x,s=e.y,r=e.z,a=Math.cos(n),o=Math.sin(n),l=Math.cos(s),c=Math.sin(s),u=Math.cos(r),d=Math.sin(r);if(e.order==="XYZ"){const h=a*u,p=a*d,g=o*u,_=o*d;t[0]=l*u,t[4]=-l*d,t[8]=c,t[1]=p+g*c,t[5]=h-_*c,t[9]=-o*l,t[2]=_-h*c,t[6]=g+p*c,t[10]=a*l}else if(e.order==="YXZ"){const h=l*u,p=l*d,g=c*u,_=c*d;t[0]=h+_*o,t[4]=g*o-p,t[8]=a*c,t[1]=a*d,t[5]=a*u,t[9]=-o,t[2]=p*o-g,t[6]=_+h*o,t[10]=a*l}else if(e.order==="ZXY"){const h=l*u,p=l*d,g=c*u,_=c*d;t[0]=h-_*o,t[4]=-a*d,t[8]=g+p*o,t[1]=p+g*o,t[5]=a*u,t[9]=_-h*o,t[2]=-a*c,t[6]=o,t[10]=a*l}else if(e.order==="ZYX"){const h=a*u,p=a*d,g=o*u,_=o*d;t[0]=l*u,t[4]=g*c-p,t[8]=h*c+_,t[1]=l*d,t[5]=_*c+h,t[9]=p*c-g,t[2]=-c,t[6]=o*l,t[10]=a*l}else if(e.order==="YZX"){const h=a*l,p=a*c,g=o*l,_=o*c;t[0]=l*u,t[4]=_-h*d,t[8]=g*d+p,t[1]=d,t[5]=a*u,t[9]=-o*u,t[2]=-c*u,t[6]=p*d+g,t[10]=h-_*d}else if(e.order==="XZY"){const h=a*l,p=a*c,g=o*l,_=o*c;t[0]=l*u,t[4]=-d,t[8]=c*u,t[1]=h*d+_,t[5]=a*u,t[9]=p*d-g,t[2]=g*d-p,t[6]=o*u,t[10]=_*d+h}return t[3]=0,t[7]=0,t[11]=0,t[12]=0,t[13]=0,t[14]=0,t[15]=1,this}makeRotationFromQuaternion(e){return this.compose(mf,e,gf)}lookAt(e,t,n){const s=this.elements;return Jt.subVectors(e,t),Jt.lengthSq()===0&&(Jt.z=1),Jt.normalize(),Bn.crossVectors(n,Jt),Bn.lengthSq()===0&&(Math.abs(n.z)===1?Jt.x+=1e-4:Jt.z+=1e-4,Jt.normalize(),Bn.crossVectors(n,Jt)),Bn.normalize(),Rs.crossVectors(Jt,Bn),s[0]=Bn.x,s[4]=Rs.x,s[8]=Jt.x,s[1]=Bn.y,s[5]=Rs.y,s[9]=Jt.y,s[2]=Bn.z,s[6]=Rs.z,s[10]=Jt.z,this}multiply(e){return this.multiplyMatrices(this,e)}premultiply(e){return this.multiplyMatrices(e,this)}multiplyMatrices(e,t){const n=e.elements,s=t.elements,r=this.elements,a=n[0],o=n[4],l=n[8],c=n[12],u=n[1],d=n[5],h=n[9],p=n[13],g=n[2],_=n[6],m=n[10],f=n[14],E=n[3],b=n[7],v=n[11],T=n[15],A=s[0],C=s[4],I=s[8],y=s[12],S=s[1],P=s[5],$=s[9],q=s[13],Y=s[2],X=s[6],B=s[10],Q=s[14],G=s[3],ie=s[7],de=s[11],Ee=s[15];return r[0]=a*A+o*S+l*Y+c*G,r[4]=a*C+o*P+l*X+c*ie,r[8]=a*I+o*$+l*B+c*de,r[12]=a*y+o*q+l*Q+c*Ee,r[1]=u*A+d*S+h*Y+p*G,r[5]=u*C+d*P+h*X+p*ie,r[9]=u*I+d*$+h*B+p*de,r[13]=u*y+d*q+h*Q+p*Ee,r[2]=g*A+_*S+m*Y+f*G,r[6]=g*C+_*P+m*X+f*ie,r[10]=g*I+_*$+m*B+f*de,r[14]=g*y+_*q+m*Q+f*Ee,r[3]=E*A+b*S+v*Y+T*G,r[7]=E*C+b*P+v*X+T*ie,r[11]=E*I+b*$+v*B+T*de,r[15]=E*y+b*q+v*Q+T*Ee,this}multiplyScalar(e){const t=this.elements;return t[0]*=e,t[4]*=e,t[8]*=e,t[12]*=e,t[1]*=e,t[5]*=e,t[9]*=e,t[13]*=e,t[2]*=e,t[6]*=e,t[10]*=e,t[14]*=e,t[3]*=e,t[7]*=e,t[11]*=e,t[15]*=e,this}determinant(){const e=this.elements,t=e[0],n=e[4],s=e[8],r=e[12],a=e[1],o=e[5],l=e[9],c=e[13],u=e[2],d=e[6],h=e[10],p=e[14],g=e[3],_=e[7],m=e[11],f=e[15];return g*(+r*l*d-s*c*d-r*o*h+n*c*h+s*o*p-n*l*p)+_*(+t*l*p-t*c*h+r*a*h-s*a*p+s*c*u-r*l*u)+m*(+t*c*d-t*o*p-r*a*d+n*a*p+r*o*u-n*c*u)+f*(-s*o*u-t*l*d+t*o*h+s*a*d-n*a*h+n*l*u)}transpose(){const e=this.elements;let t;return t=e[1],e[1]=e[4],e[4]=t,t=e[2],e[2]=e[8],e[8]=t,t=e[6],e[6]=e[9],e[9]=t,t=e[3],e[3]=e[12],e[12]=t,t=e[7],e[7]=e[13],e[13]=t,t=e[11],e[11]=e[14],e[14]=t,this}setPosition(e,t,n){const s=this.elements;return e.isVector3?(s[12]=e.x,s[13]=e.y,s[14]=e.z):(s[12]=e,s[13]=t,s[14]=n),this}invert(){const e=this.elements,t=e[0],n=e[1],s=e[2],r=e[3],a=e[4],o=e[5],l=e[6],c=e[7],u=e[8],d=e[9],h=e[10],p=e[11],g=e[12],_=e[13],m=e[14],f=e[15],E=d*m*c-_*h*c+_*l*p-o*m*p-d*l*f+o*h*f,b=g*h*c-u*m*c-g*l*p+a*m*p+u*l*f-a*h*f,v=u*_*c-g*d*c+g*o*p-a*_*p-u*o*f+a*d*f,T=g*d*l-u*_*l-g*o*h+a*_*h+u*o*m-a*d*m,A=t*E+n*b+s*v+r*T;if(A===0)return this.set(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);const C=1/A;return e[0]=E*C,e[1]=(_*h*r-d*m*r-_*s*p+n*m*p+d*s*f-n*h*f)*C,e[2]=(o*m*r-_*l*r+_*s*c-n*m*c-o*s*f+n*l*f)*C,e[3]=(d*l*r-o*h*r-d*s*c+n*h*c+o*s*p-n*l*p)*C,e[4]=b*C,e[5]=(u*m*r-g*h*r+g*s*p-t*m*p-u*s*f+t*h*f)*C,e[6]=(g*l*r-a*m*r-g*s*c+t*m*c+a*s*f-t*l*f)*C,e[7]=(a*h*r-u*l*r+u*s*c-t*h*c-a*s*p+t*l*p)*C,e[8]=v*C,e[9]=(g*d*r-u*_*r-g*n*p+t*_*p+u*n*f-t*d*f)*C,e[10]=(a*_*r-g*o*r+g*n*c-t*_*c-a*n*f+t*o*f)*C,e[11]=(u*o*r-a*d*r-u*n*c+t*d*c+a*n*p-t*o*p)*C,e[12]=T*C,e[13]=(u*_*s-g*d*s+g*n*h-t*_*h-u*n*m+t*d*m)*C,e[14]=(g*o*s-a*_*s-g*n*l+t*_*l+a*n*m-t*o*m)*C,e[15]=(a*d*s-u*o*s+u*n*l-t*d*l-a*n*h+t*o*h)*C,this}scale(e){const t=this.elements,n=e.x,s=e.y,r=e.z;return t[0]*=n,t[4]*=s,t[8]*=r,t[1]*=n,t[5]*=s,t[9]*=r,t[2]*=n,t[6]*=s,t[10]*=r,t[3]*=n,t[7]*=s,t[11]*=r,this}getMaxScaleOnAxis(){const e=this.elements,t=e[0]*e[0]+e[1]*e[1]+e[2]*e[2],n=e[4]*e[4]+e[5]*e[5]+e[6]*e[6],s=e[8]*e[8]+e[9]*e[9]+e[10]*e[10];return Math.sqrt(Math.max(t,n,s))}makeTranslation(e,t,n){return e.isVector3?this.set(1,0,0,e.x,0,1,0,e.y,0,0,1,e.z,0,0,0,1):this.set(1,0,0,e,0,1,0,t,0,0,1,n,0,0,0,1),this}makeRotationX(e){const t=Math.cos(e),n=Math.sin(e);return this.set(1,0,0,0,0,t,-n,0,0,n,t,0,0,0,0,1),this}makeRotationY(e){const t=Math.cos(e),n=Math.sin(e);return this.set(t,0,n,0,0,1,0,0,-n,0,t,0,0,0,0,1),this}makeRotationZ(e){const t=Math.cos(e),n=Math.sin(e);return this.set(t,-n,0,0,n,t,0,0,0,0,1,0,0,0,0,1),this}makeRotationAxis(e,t){const n=Math.cos(t),s=Math.sin(t),r=1-n,a=e.x,o=e.y,l=e.z,c=r*a,u=r*o;return this.set(c*a+n,c*o-s*l,c*l+s*o,0,c*o+s*l,u*o+n,u*l-s*a,0,c*l-s*o,u*l+s*a,r*l*l+n,0,0,0,0,1),this}makeScale(e,t,n){return this.set(e,0,0,0,0,t,0,0,0,0,n,0,0,0,0,1),this}makeShear(e,t,n,s,r,a){return this.set(1,n,r,0,e,1,a,0,t,s,1,0,0,0,0,1),this}compose(e,t,n){const s=this.elements,r=t._x,a=t._y,o=t._z,l=t._w,c=r+r,u=a+a,d=o+o,h=r*c,p=r*u,g=r*d,_=a*u,m=a*d,f=o*d,E=l*c,b=l*u,v=l*d,T=n.x,A=n.y,C=n.z;return s[0]=(1-(_+f))*T,s[1]=(p+v)*T,s[2]=(g-b)*T,s[3]=0,s[4]=(p-v)*A,s[5]=(1-(h+f))*A,s[6]=(m+E)*A,s[7]=0,s[8]=(g+b)*C,s[9]=(m-E)*C,s[10]=(1-(h+_))*C,s[11]=0,s[12]=e.x,s[13]=e.y,s[14]=e.z,s[15]=1,this}decompose(e,t,n){const s=this.elements;let r=yi.set(s[0],s[1],s[2]).length();const a=yi.set(s[4],s[5],s[6]).length(),o=yi.set(s[8],s[9],s[10]).length();this.determinant()<0&&(r=-r),e.x=s[12],e.y=s[13],e.z=s[14],cn.copy(this);const c=1/r,u=1/a,d=1/o;return cn.elements[0]*=c,cn.elements[1]*=c,cn.elements[2]*=c,cn.elements[4]*=u,cn.elements[5]*=u,cn.elements[6]*=u,cn.elements[8]*=d,cn.elements[9]*=d,cn.elements[10]*=d,t.setFromRotationMatrix(cn),n.x=r,n.y=a,n.z=o,this}makePerspective(e,t,n,s,r,a,o=_n,l=!1){const c=this.elements,u=2*r/(t-e),d=2*r/(n-s),h=(t+e)/(t-e),p=(n+s)/(n-s);let g,_;if(l)g=r/(a-r),_=a*r/(a-r);else if(o===_n)g=-(a+r)/(a-r),_=-2*a*r/(a-r);else if(o===ar)g=-a/(a-r),_=-a*r/(a-r);else throw new Error("THREE.Matrix4.makePerspective(): Invalid coordinate system: "+o);return c[0]=u,c[4]=0,c[8]=h,c[12]=0,c[1]=0,c[5]=d,c[9]=p,c[13]=0,c[2]=0,c[6]=0,c[10]=g,c[14]=_,c[3]=0,c[7]=0,c[11]=-1,c[15]=0,this}makeOrthographic(e,t,n,s,r,a,o=_n,l=!1){const c=this.elements,u=2/(t-e),d=2/(n-s),h=-(t+e)/(t-e),p=-(n+s)/(n-s);let g,_;if(l)g=1/(a-r),_=a/(a-r);else if(o===_n)g=-2/(a-r),_=-(a+r)/(a-r);else if(o===ar)g=-1/(a-r),_=-r/(a-r);else throw new Error("THREE.Matrix4.makeOrthographic(): Invalid coordinate system: "+o);return c[0]=u,c[4]=0,c[8]=0,c[12]=h,c[1]=0,c[5]=d,c[9]=0,c[13]=p,c[2]=0,c[6]=0,c[10]=g,c[14]=_,c[3]=0,c[7]=0,c[11]=0,c[15]=1,this}equals(e){const t=this.elements,n=e.elements;for(let s=0;s<16;s++)if(t[s]!==n[s])return!1;return!0}fromArray(e,t=0){for(let n=0;n<16;n++)this.elements[n]=e[n+t];return this}toArray(e=[],t=0){const n=this.elements;return e[t]=n[0],e[t+1]=n[1],e[t+2]=n[2],e[t+3]=n[3],e[t+4]=n[4],e[t+5]=n[5],e[t+6]=n[6],e[t+7]=n[7],e[t+8]=n[8],e[t+9]=n[9],e[t+10]=n[10],e[t+11]=n[11],e[t+12]=n[12],e[t+13]=n[13],e[t+14]=n[14],e[t+15]=n[15],e}}const yi=new O,cn=new bt,mf=new O(0,0,0),gf=new O(1,1,1),Bn=new O,Rs=new O,Jt=new O,dl=new bt,hl=new ms;class Sn{constructor(e=0,t=0,n=0,s=Sn.DEFAULT_ORDER){this.isEuler=!0,this._x=e,this._y=t,this._z=n,this._order=s}get x(){return this._x}set x(e){this._x=e,this._onChangeCallback()}get y(){return this._y}set y(e){this._y=e,this._onChangeCallback()}get z(){return this._z}set z(e){this._z=e,this._onChangeCallback()}get order(){return this._order}set order(e){this._order=e,this._onChangeCallback()}set(e,t,n,s=this._order){return this._x=e,this._y=t,this._z=n,this._order=s,this._onChangeCallback(),this}clone(){return new this.constructor(this._x,this._y,this._z,this._order)}copy(e){return this._x=e._x,this._y=e._y,this._z=e._z,this._order=e._order,this._onChangeCallback(),this}setFromRotationMatrix(e,t=this._order,n=!0){const s=e.elements,r=s[0],a=s[4],o=s[8],l=s[1],c=s[5],u=s[9],d=s[2],h=s[6],p=s[10];switch(t){case"XYZ":this._y=Math.asin(it(o,-1,1)),Math.abs(o)<.9999999?(this._x=Math.atan2(-u,p),this._z=Math.atan2(-a,r)):(this._x=Math.atan2(h,c),this._z=0);break;case"YXZ":this._x=Math.asin(-it(u,-1,1)),Math.abs(u)<.9999999?(this._y=Math.atan2(o,p),this._z=Math.atan2(l,c)):(this._y=Math.atan2(-d,r),this._z=0);break;case"ZXY":this._x=Math.asin(it(h,-1,1)),Math.abs(h)<.9999999?(this._y=Math.atan2(-d,p),this._z=Math.atan2(-a,c)):(this._y=0,this._z=Math.atan2(l,r));break;case"ZYX":this._y=Math.asin(-it(d,-1,1)),Math.abs(d)<.9999999?(this._x=Math.atan2(h,p),this._z=Math.atan2(l,r)):(this._x=0,this._z=Math.atan2(-a,c));break;case"YZX":this._z=Math.asin(it(l,-1,1)),Math.abs(l)<.9999999?(this._x=Math.atan2(-u,c),this._y=Math.atan2(-d,r)):(this._x=0,this._y=Math.atan2(o,p));break;case"XZY":this._z=Math.asin(-it(a,-1,1)),Math.abs(a)<.9999999?(this._x=Math.atan2(h,c),this._y=Math.atan2(o,r)):(this._x=Math.atan2(-u,p),this._y=0);break;default:console.warn("THREE.Euler: .setFromRotationMatrix() encountered an unknown order: "+t)}return this._order=t,n===!0&&this._onChangeCallback(),this}setFromQuaternion(e,t,n){return dl.makeRotationFromQuaternion(e),this.setFromRotationMatrix(dl,t,n)}setFromVector3(e,t=this._order){return this.set(e.x,e.y,e.z,t)}reorder(e){return hl.setFromEuler(this),this.setFromQuaternion(hl,e)}equals(e){return e._x===this._x&&e._y===this._y&&e._z===this._z&&e._order===this._order}fromArray(e){return this._x=e[0],this._y=e[1],this._z=e[2],e[3]!==void 0&&(this._order=e[3]),this._onChangeCallback(),this}toArray(e=[],t=0){return e[t]=this._x,e[t+1]=this._y,e[t+2]=this._z,e[t+3]=this._order,e}_onChange(e){return this._onChangeCallback=e,this}_onChangeCallback(){}*[Symbol.iterator](){yield this._x,yield this._y,yield this._z,yield this._order}}Sn.DEFAULT_ORDER="XYZ";class Ao{constructor(){this.mask=1}set(e){this.mask=(1<<e|0)>>>0}enable(e){this.mask|=1<<e|0}enableAll(){this.mask=-1}toggle(e){this.mask^=1<<e|0}disable(e){this.mask&=~(1<<e|0)}disableAll(){this.mask=0}test(e){return(this.mask&e.mask)!==0}isEnabled(e){return(this.mask&(1<<e|0))!==0}}let _f=0;const fl=new O,bi=new ms,Tn=new bt,Cs=new O,Zi=new O,vf=new O,xf=new ms,pl=new O(1,0,0),ml=new O(0,1,0),gl=new O(0,0,1),_l={type:"added"},Mf={type:"removed"},Ei={type:"childadded",child:null},Br={type:"childremoved",child:null};class Ut extends Wi{constructor(){super(),this.isObject3D=!0,Object.defineProperty(this,"id",{value:_f++}),this.uuid=$i(),this.name="",this.type="Object3D",this.parent=null,this.children=[],this.up=Ut.DEFAULT_UP.clone();const e=new O,t=new Sn,n=new ms,s=new O(1,1,1);function r(){n.setFromEuler(t,!1)}function a(){t.setFromQuaternion(n,void 0,!1)}t._onChange(r),n._onChange(a),Object.defineProperties(this,{position:{configurable:!0,enumerable:!0,value:e},rotation:{configurable:!0,enumerable:!0,value:t},quaternion:{configurable:!0,enumerable:!0,value:n},scale:{configurable:!0,enumerable:!0,value:s},modelViewMatrix:{value:new bt},normalMatrix:{value:new Ze}}),this.matrix=new bt,this.matrixWorld=new bt,this.matrixAutoUpdate=Ut.DEFAULT_MATRIX_AUTO_UPDATE,this.matrixWorldAutoUpdate=Ut.DEFAULT_MATRIX_WORLD_AUTO_UPDATE,this.matrixWorldNeedsUpdate=!1,this.layers=new Ao,this.visible=!0,this.castShadow=!1,this.receiveShadow=!1,this.frustumCulled=!0,this.renderOrder=0,this.animations=[],this.customDepthMaterial=void 0,this.customDistanceMaterial=void 0,this.userData={}}onBeforeShadow(){}onAfterShadow(){}onBeforeRender(){}onAfterRender(){}applyMatrix4(e){this.matrixAutoUpdate&&this.updateMatrix(),this.matrix.premultiply(e),this.matrix.decompose(this.position,this.quaternion,this.scale)}applyQuaternion(e){return this.quaternion.premultiply(e),this}setRotationFromAxisAngle(e,t){this.quaternion.setFromAxisAngle(e,t)}setRotationFromEuler(e){this.quaternion.setFromEuler(e,!0)}setRotationFromMatrix(e){this.quaternion.setFromRotationMatrix(e)}setRotationFromQuaternion(e){this.quaternion.copy(e)}rotateOnAxis(e,t){return bi.setFromAxisAngle(e,t),this.quaternion.multiply(bi),this}rotateOnWorldAxis(e,t){return bi.setFromAxisAngle(e,t),this.quaternion.premultiply(bi),this}rotateX(e){return this.rotateOnAxis(pl,e)}rotateY(e){return this.rotateOnAxis(ml,e)}rotateZ(e){return this.rotateOnAxis(gl,e)}translateOnAxis(e,t){return fl.copy(e).applyQuaternion(this.quaternion),this.position.add(fl.multiplyScalar(t)),this}translateX(e){return this.translateOnAxis(pl,e)}translateY(e){return this.translateOnAxis(ml,e)}translateZ(e){return this.translateOnAxis(gl,e)}localToWorld(e){return this.updateWorldMatrix(!0,!1),e.applyMatrix4(this.matrixWorld)}worldToLocal(e){return this.updateWorldMatrix(!0,!1),e.applyMatrix4(Tn.copy(this.matrixWorld).invert())}lookAt(e,t,n){e.isVector3?Cs.copy(e):Cs.set(e,t,n);const s=this.parent;this.updateWorldMatrix(!0,!1),Zi.setFromMatrixPosition(this.matrixWorld),this.isCamera||this.isLight?Tn.lookAt(Zi,Cs,this.up):Tn.lookAt(Cs,Zi,this.up),this.quaternion.setFromRotationMatrix(Tn),s&&(Tn.extractRotation(s.matrixWorld),bi.setFromRotationMatrix(Tn),this.quaternion.premultiply(bi.invert()))}add(e){if(arguments.length>1){for(let t=0;t<arguments.length;t++)this.add(arguments[t]);return this}return e===this?(console.error("THREE.Object3D.add: object can't be added as a child of itself.",e),this):(e&&e.isObject3D?(e.removeFromParent(),e.parent=this,this.children.push(e),e.dispatchEvent(_l),Ei.child=e,this.dispatchEvent(Ei),Ei.child=null):console.error("THREE.Object3D.add: object not an instance of THREE.Object3D.",e),this)}remove(e){if(arguments.length>1){for(let n=0;n<arguments.length;n++)this.remove(arguments[n]);return this}const t=this.children.indexOf(e);return t!==-1&&(e.parent=null,this.children.splice(t,1),e.dispatchEvent(Mf),Br.child=e,this.dispatchEvent(Br),Br.child=null),this}removeFromParent(){const e=this.parent;return e!==null&&e.remove(this),this}clear(){return this.remove(...this.children)}attach(e){return this.updateWorldMatrix(!0,!1),Tn.copy(this.matrixWorld).invert(),e.parent!==null&&(e.parent.updateWorldMatrix(!0,!1),Tn.multiply(e.parent.matrixWorld)),e.applyMatrix4(Tn),e.removeFromParent(),e.parent=this,this.children.push(e),e.updateWorldMatrix(!1,!0),e.dispatchEvent(_l),Ei.child=e,this.dispatchEvent(Ei),Ei.child=null,this}getObjectById(e){return this.getObjectByProperty("id",e)}getObjectByName(e){return this.getObjectByProperty("name",e)}getObjectByProperty(e,t){if(this[e]===t)return this;for(let n=0,s=this.children.length;n<s;n++){const a=this.children[n].getObjectByProperty(e,t);if(a!==void 0)return a}}getObjectsByProperty(e,t,n=[]){this[e]===t&&n.push(this);const s=this.children;for(let r=0,a=s.length;r<a;r++)s[r].getObjectsByProperty(e,t,n);return n}getWorldPosition(e){return this.updateWorldMatrix(!0,!1),e.setFromMatrixPosition(this.matrixWorld)}getWorldQuaternion(e){return this.updateWorldMatrix(!0,!1),this.matrixWorld.decompose(Zi,e,vf),e}getWorldScale(e){return this.updateWorldMatrix(!0,!1),this.matrixWorld.decompose(Zi,xf,e),e}getWorldDirection(e){this.updateWorldMatrix(!0,!1);const t=this.matrixWorld.elements;return e.set(t[8],t[9],t[10]).normalize()}raycast(){}traverse(e){e(this);const t=this.children;for(let n=0,s=t.length;n<s;n++)t[n].traverse(e)}traverseVisible(e){if(this.visible===!1)return;e(this);const t=this.children;for(let n=0,s=t.length;n<s;n++)t[n].traverseVisible(e)}traverseAncestors(e){const t=this.parent;t!==null&&(e(t),t.traverseAncestors(e))}updateMatrix(){this.matrix.compose(this.position,this.quaternion,this.scale),this.matrixWorldNeedsUpdate=!0}updateMatrixWorld(e){this.matrixAutoUpdate&&this.updateMatrix(),(this.matrixWorldNeedsUpdate||e)&&(this.matrixWorldAutoUpdate===!0&&(this.parent===null?this.matrixWorld.copy(this.matrix):this.matrixWorld.multiplyMatrices(this.parent.matrixWorld,this.matrix)),this.matrixWorldNeedsUpdate=!1,e=!0);const t=this.children;for(let n=0,s=t.length;n<s;n++)t[n].updateMatrixWorld(e)}updateWorldMatrix(e,t){const n=this.parent;if(e===!0&&n!==null&&n.updateWorldMatrix(!0,!1),this.matrixAutoUpdate&&this.updateMatrix(),this.matrixWorldAutoUpdate===!0&&(this.parent===null?this.matrixWorld.copy(this.matrix):this.matrixWorld.multiplyMatrices(this.parent.matrixWorld,this.matrix)),t===!0){const s=this.children;for(let r=0,a=s.length;r<a;r++)s[r].updateWorldMatrix(!1,!0)}}toJSON(e){const t=e===void 0||typeof e=="string",n={};t&&(e={geometries:{},materials:{},textures:{},images:{},shapes:{},skeletons:{},animations:{},nodes:{}},n.metadata={version:4.7,type:"Object",generator:"Object3D.toJSON"});const s={};s.uuid=this.uuid,s.type=this.type,this.name!==""&&(s.name=this.name),this.castShadow===!0&&(s.castShadow=!0),this.receiveShadow===!0&&(s.receiveShadow=!0),this.visible===!1&&(s.visible=!1),this.frustumCulled===!1&&(s.frustumCulled=!1),this.renderOrder!==0&&(s.renderOrder=this.renderOrder),Object.keys(this.userData).length>0&&(s.userData=this.userData),s.layers=this.layers.mask,s.matrix=this.matrix.toArray(),s.up=this.up.toArray(),this.matrixAutoUpdate===!1&&(s.matrixAutoUpdate=!1),this.isInstancedMesh&&(s.type="InstancedMesh",s.count=this.count,s.instanceMatrix=this.instanceMatrix.toJSON(),this.instanceColor!==null&&(s.instanceColor=this.instanceColor.toJSON())),this.isBatchedMesh&&(s.type="BatchedMesh",s.perObjectFrustumCulled=this.perObjectFrustumCulled,s.sortObjects=this.sortObjects,s.drawRanges=this._drawRanges,s.reservedRanges=this._reservedRanges,s.geometryInfo=this._geometryInfo.map(o=>({...o,boundingBox:o.boundingBox?o.boundingBox.toJSON():void 0,boundingSphere:o.boundingSphere?o.boundingSphere.toJSON():void 0})),s.instanceInfo=this._instanceInfo.map(o=>({...o})),s.availableInstanceIds=this._availableInstanceIds.slice(),s.availableGeometryIds=this._availableGeometryIds.slice(),s.nextIndexStart=this._nextIndexStart,s.nextVertexStart=this._nextVertexStart,s.geometryCount=this._geometryCount,s.maxInstanceCount=this._maxInstanceCount,s.maxVertexCount=this._maxVertexCount,s.maxIndexCount=this._maxIndexCount,s.geometryInitialized=this._geometryInitialized,s.matricesTexture=this._matricesTexture.toJSON(e),s.indirectTexture=this._indirectTexture.toJSON(e),this._colorsTexture!==null&&(s.colorsTexture=this._colorsTexture.toJSON(e)),this.boundingSphere!==null&&(s.boundingSphere=this.boundingSphere.toJSON()),this.boundingBox!==null&&(s.boundingBox=this.boundingBox.toJSON()));function r(o,l){return o[l.uuid]===void 0&&(o[l.uuid]=l.toJSON(e)),l.uuid}if(this.isScene)this.background&&(this.background.isColor?s.background=this.background.toJSON():this.background.isTexture&&(s.background=this.background.toJSON(e).uuid)),this.environment&&this.environment.isTexture&&this.environment.isRenderTargetTexture!==!0&&(s.environment=this.environment.toJSON(e).uuid);else if(this.isMesh||this.isLine||this.isPoints){s.geometry=r(e.geometries,this.geometry);const o=this.geometry.parameters;if(o!==void 0&&o.shapes!==void 0){const l=o.shapes;if(Array.isArray(l))for(let c=0,u=l.length;c<u;c++){const d=l[c];r(e.shapes,d)}else r(e.shapes,l)}}if(this.isSkinnedMesh&&(s.bindMode=this.bindMode,s.bindMatrix=this.bindMatrix.toArray(),this.skeleton!==void 0&&(r(e.skeletons,this.skeleton),s.skeleton=this.skeleton.uuid)),this.material!==void 0)if(Array.isArray(this.material)){const o=[];for(let l=0,c=this.material.length;l<c;l++)o.push(r(e.materials,this.material[l]));s.material=o}else s.material=r(e.materials,this.material);if(this.children.length>0){s.children=[];for(let o=0;o<this.children.length;o++)s.children.push(this.children[o].toJSON(e).object)}if(this.animations.length>0){s.animations=[];for(let o=0;o<this.animations.length;o++){const l=this.animations[o];s.animations.push(r(e.animations,l))}}if(t){const o=a(e.geometries),l=a(e.materials),c=a(e.textures),u=a(e.images),d=a(e.shapes),h=a(e.skeletons),p=a(e.animations),g=a(e.nodes);o.length>0&&(n.geometries=o),l.length>0&&(n.materials=l),c.length>0&&(n.textures=c),u.length>0&&(n.images=u),d.length>0&&(n.shapes=d),h.length>0&&(n.skeletons=h),p.length>0&&(n.animations=p),g.length>0&&(n.nodes=g)}return n.object=s,n;function a(o){const l=[];for(const c in o){const u=o[c];delete u.metadata,l.push(u)}return l}}clone(e){return new this.constructor().copy(this,e)}copy(e,t=!0){if(this.name=e.name,this.up.copy(e.up),this.position.copy(e.position),this.rotation.order=e.rotation.order,this.quaternion.copy(e.quaternion),this.scale.copy(e.scale),this.matrix.copy(e.matrix),this.matrixWorld.copy(e.matrixWorld),this.matrixAutoUpdate=e.matrixAutoUpdate,this.matrixWorldAutoUpdate=e.matrixWorldAutoUpdate,this.matrixWorldNeedsUpdate=e.matrixWorldNeedsUpdate,this.layers.mask=e.layers.mask,this.visible=e.visible,this.castShadow=e.castShadow,this.receiveShadow=e.receiveShadow,this.frustumCulled=e.frustumCulled,this.renderOrder=e.renderOrder,this.animations=e.animations.slice(),this.userData=JSON.parse(JSON.stringify(e.userData)),t===!0)for(let n=0;n<e.children.length;n++){const s=e.children[n];this.add(s.clone())}return this}}Ut.DEFAULT_UP=new O(0,1,0);Ut.DEFAULT_MATRIX_AUTO_UPDATE=!0;Ut.DEFAULT_MATRIX_WORLD_AUTO_UPDATE=!0;const un=new O,wn=new O,zr=new O,An=new O,Ti=new O,wi=new O,vl=new O,Vr=new O,Hr=new O,Gr=new O,Wr=new At,$r=new At,Xr=new At;class dn{constructor(e=new O,t=new O,n=new O){this.a=e,this.b=t,this.c=n}static getNormal(e,t,n,s){s.subVectors(n,t),un.subVectors(e,t),s.cross(un);const r=s.lengthSq();return r>0?s.multiplyScalar(1/Math.sqrt(r)):s.set(0,0,0)}static getBarycoord(e,t,n,s,r){un.subVectors(s,t),wn.subVectors(n,t),zr.subVectors(e,t);const a=un.dot(un),o=un.dot(wn),l=un.dot(zr),c=wn.dot(wn),u=wn.dot(zr),d=a*c-o*o;if(d===0)return r.set(0,0,0),null;const h=1/d,p=(c*l-o*u)*h,g=(a*u-o*l)*h;return r.set(1-p-g,g,p)}static containsPoint(e,t,n,s){return this.getBarycoord(e,t,n,s,An)===null?!1:An.x>=0&&An.y>=0&&An.x+An.y<=1}static getInterpolation(e,t,n,s,r,a,o,l){return this.getBarycoord(e,t,n,s,An)===null?(l.x=0,l.y=0,"z"in l&&(l.z=0),"w"in l&&(l.w=0),null):(l.setScalar(0),l.addScaledVector(r,An.x),l.addScaledVector(a,An.y),l.addScaledVector(o,An.z),l)}static getInterpolatedAttribute(e,t,n,s,r,a){return Wr.setScalar(0),$r.setScalar(0),Xr.setScalar(0),Wr.fromBufferAttribute(e,t),$r.fromBufferAttribute(e,n),Xr.fromBufferAttribute(e,s),a.setScalar(0),a.addScaledVector(Wr,r.x),a.addScaledVector($r,r.y),a.addScaledVector(Xr,r.z),a}static isFrontFacing(e,t,n,s){return un.subVectors(n,t),wn.subVectors(e,t),un.cross(wn).dot(s)<0}set(e,t,n){return this.a.copy(e),this.b.copy(t),this.c.copy(n),this}setFromPointsAndIndices(e,t,n,s){return this.a.copy(e[t]),this.b.copy(e[n]),this.c.copy(e[s]),this}setFromAttributeAndIndices(e,t,n,s){return this.a.fromBufferAttribute(e,t),this.b.fromBufferAttribute(e,n),this.c.fromBufferAttribute(e,s),this}clone(){return new this.constructor().copy(this)}copy(e){return this.a.copy(e.a),this.b.copy(e.b),this.c.copy(e.c),this}getArea(){return un.subVectors(this.c,this.b),wn.subVectors(this.a,this.b),un.cross(wn).length()*.5}getMidpoint(e){return e.addVectors(this.a,this.b).add(this.c).multiplyScalar(1/3)}getNormal(e){return dn.getNormal(this.a,this.b,this.c,e)}getPlane(e){return e.setFromCoplanarPoints(this.a,this.b,this.c)}getBarycoord(e,t){return dn.getBarycoord(e,this.a,this.b,this.c,t)}getInterpolation(e,t,n,s,r){return dn.getInterpolation(e,this.a,this.b,this.c,t,n,s,r)}containsPoint(e){return dn.containsPoint(e,this.a,this.b,this.c)}isFrontFacing(e){return dn.isFrontFacing(this.a,this.b,this.c,e)}intersectsBox(e){return e.intersectsTriangle(this)}closestPointToPoint(e,t){const n=this.a,s=this.b,r=this.c;let a,o;Ti.subVectors(s,n),wi.subVectors(r,n),Vr.subVectors(e,n);const l=Ti.dot(Vr),c=wi.dot(Vr);if(l<=0&&c<=0)return t.copy(n);Hr.subVectors(e,s);const u=Ti.dot(Hr),d=wi.dot(Hr);if(u>=0&&d<=u)return t.copy(s);const h=l*d-u*c;if(h<=0&&l>=0&&u<=0)return a=l/(l-u),t.copy(n).addScaledVector(Ti,a);Gr.subVectors(e,r);const p=Ti.dot(Gr),g=wi.dot(Gr);if(g>=0&&p<=g)return t.copy(r);const _=p*c-l*g;if(_<=0&&c>=0&&g<=0)return o=c/(c-g),t.copy(n).addScaledVector(wi,o);const m=u*g-p*d;if(m<=0&&d-u>=0&&p-g>=0)return vl.subVectors(r,s),o=(d-u)/(d-u+(p-g)),t.copy(s).addScaledVector(vl,o);const f=1/(m+_+h);return a=_*f,o=h*f,t.copy(n).addScaledVector(Ti,a).addScaledVector(wi,o)}equals(e){return e.a.equals(this.a)&&e.b.equals(this.b)&&e.c.equals(this.c)}}const Ic={aliceblue:15792383,antiquewhite:16444375,aqua:65535,aquamarine:8388564,azure:15794175,beige:16119260,bisque:16770244,black:0,blanchedalmond:16772045,blue:255,blueviolet:9055202,brown:10824234,burlywood:14596231,cadetblue:6266528,chartreuse:8388352,chocolate:13789470,coral:16744272,cornflowerblue:6591981,cornsilk:16775388,crimson:14423100,cyan:65535,darkblue:139,darkcyan:35723,darkgoldenrod:12092939,darkgray:11119017,darkgreen:25600,darkgrey:11119017,darkkhaki:12433259,darkmagenta:9109643,darkolivegreen:5597999,darkorange:16747520,darkorchid:10040012,darkred:9109504,darksalmon:15308410,darkseagreen:9419919,darkslateblue:4734347,darkslategray:3100495,darkslategrey:3100495,darkturquoise:52945,darkviolet:9699539,deeppink:16716947,deepskyblue:49151,dimgray:6908265,dimgrey:6908265,dodgerblue:2003199,firebrick:11674146,floralwhite:16775920,forestgreen:2263842,fuchsia:16711935,gainsboro:14474460,ghostwhite:16316671,gold:16766720,goldenrod:14329120,gray:8421504,green:32768,greenyellow:11403055,grey:8421504,honeydew:15794160,hotpink:16738740,indianred:13458524,indigo:4915330,ivory:16777200,khaki:15787660,lavender:15132410,lavenderblush:16773365,lawngreen:8190976,lemonchiffon:16775885,lightblue:11393254,lightcoral:15761536,lightcyan:14745599,lightgoldenrodyellow:16448210,lightgray:13882323,lightgreen:9498256,lightgrey:13882323,lightpink:16758465,lightsalmon:16752762,lightseagreen:2142890,lightskyblue:8900346,lightslategray:7833753,lightslategrey:7833753,lightsteelblue:11584734,lightyellow:16777184,lime:65280,limegreen:3329330,linen:16445670,magenta:16711935,maroon:8388608,mediumaquamarine:6737322,mediumblue:205,mediumorchid:12211667,mediumpurple:9662683,mediumseagreen:3978097,mediumslateblue:8087790,mediumspringgreen:64154,mediumturquoise:4772300,mediumvioletred:13047173,midnightblue:1644912,mintcream:16121850,mistyrose:16770273,moccasin:16770229,navajowhite:16768685,navy:128,oldlace:16643558,olive:8421376,olivedrab:7048739,orange:16753920,orangered:16729344,orchid:14315734,palegoldenrod:15657130,palegreen:10025880,paleturquoise:11529966,palevioletred:14381203,papayawhip:16773077,peachpuff:16767673,peru:13468991,pink:16761035,plum:14524637,powderblue:11591910,purple:8388736,rebeccapurple:6697881,red:16711680,rosybrown:12357519,royalblue:4286945,saddlebrown:9127187,salmon:16416882,sandybrown:16032864,seagreen:3050327,seashell:16774638,sienna:10506797,silver:12632256,skyblue:8900331,slateblue:6970061,slategray:7372944,slategrey:7372944,snow:16775930,springgreen:65407,steelblue:4620980,tan:13808780,teal:32896,thistle:14204888,tomato:16737095,turquoise:4251856,violet:15631086,wheat:16113331,white:16777215,whitesmoke:16119285,yellow:16776960,yellowgreen:10145074},zn={h:0,s:0,l:0},Ps={h:0,s:0,l:0};function qr(i,e,t){return t<0&&(t+=1),t>1&&(t-=1),t<1/6?i+(e-i)*6*t:t<1/2?e:t<2/3?i+(e-i)*6*(2/3-t):i}class Le{constructor(e,t,n){return this.isColor=!0,this.r=1,this.g=1,this.b=1,this.set(e,t,n)}set(e,t,n){if(t===void 0&&n===void 0){const s=e;s&&s.isColor?this.copy(s):typeof s=="number"?this.setHex(s):typeof s=="string"&&this.setStyle(s)}else this.setRGB(e,t,n);return this}setScalar(e){return this.r=e,this.g=e,this.b=e,this}setHex(e,t=rn){return e=Math.floor(e),this.r=(e>>16&255)/255,this.g=(e>>8&255)/255,this.b=(e&255)/255,lt.colorSpaceToWorking(this,t),this}setRGB(e,t,n,s=lt.workingColorSpace){return this.r=e,this.g=t,this.b=n,lt.colorSpaceToWorking(this,s),this}setHSL(e,t,n,s=lt.workingColorSpace){if(e=To(e,1),t=it(t,0,1),n=it(n,0,1),t===0)this.r=this.g=this.b=n;else{const r=n<=.5?n*(1+t):n+t-n*t,a=2*n-r;this.r=qr(a,r,e+1/3),this.g=qr(a,r,e),this.b=qr(a,r,e-1/3)}return lt.colorSpaceToWorking(this,s),this}setStyle(e,t=rn){function n(r){r!==void 0&&parseFloat(r)<1&&console.warn("THREE.Color: Alpha component of "+e+" will be ignored.")}let s;if(s=/^(\w+)\(([^\)]*)\)/.exec(e)){let r;const a=s[1],o=s[2];switch(a){case"rgb":case"rgba":if(r=/^\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*(?:,\s*(\d*\.?\d+)\s*)?$/.exec(o))return n(r[4]),this.setRGB(Math.min(255,parseInt(r[1],10))/255,Math.min(255,parseInt(r[2],10))/255,Math.min(255,parseInt(r[3],10))/255,t);if(r=/^\s*(\d+)\%\s*,\s*(\d+)\%\s*,\s*(\d+)\%\s*(?:,\s*(\d*\.?\d+)\s*)?$/.exec(o))return n(r[4]),this.setRGB(Math.min(100,parseInt(r[1],10))/100,Math.min(100,parseInt(r[2],10))/100,Math.min(100,parseInt(r[3],10))/100,t);break;case"hsl":case"hsla":if(r=/^\s*(\d*\.?\d+)\s*,\s*(\d*\.?\d+)\%\s*,\s*(\d*\.?\d+)\%\s*(?:,\s*(\d*\.?\d+)\s*)?$/.exec(o))return n(r[4]),this.setHSL(parseFloat(r[1])/360,parseFloat(r[2])/100,parseFloat(r[3])/100,t);break;default:console.warn("THREE.Color: Unknown color model "+e)}}else if(s=/^\#([A-Fa-f\d]+)$/.exec(e)){const r=s[1],a=r.length;if(a===3)return this.setRGB(parseInt(r.charAt(0),16)/15,parseInt(r.charAt(1),16)/15,parseInt(r.charAt(2),16)/15,t);if(a===6)return this.setHex(parseInt(r,16),t);console.warn("THREE.Color: Invalid hex color "+e)}else if(e&&e.length>0)return this.setColorName(e,t);return this}setColorName(e,t=rn){const n=Ic[e.toLowerCase()];return n!==void 0?this.setHex(n,t):console.warn("THREE.Color: Unknown color "+e),this}clone(){return new this.constructor(this.r,this.g,this.b)}copy(e){return this.r=e.r,this.g=e.g,this.b=e.b,this}copySRGBToLinear(e){return this.r=In(e.r),this.g=In(e.g),this.b=In(e.b),this}copyLinearToSRGB(e){return this.r=Fi(e.r),this.g=Fi(e.g),this.b=Fi(e.b),this}convertSRGBToLinear(){return this.copySRGBToLinear(this),this}convertLinearToSRGB(){return this.copyLinearToSRGB(this),this}getHex(e=rn){return lt.workingToColorSpace(Ot.copy(this),e),Math.round(it(Ot.r*255,0,255))*65536+Math.round(it(Ot.g*255,0,255))*256+Math.round(it(Ot.b*255,0,255))}getHexString(e=rn){return("000000"+this.getHex(e).toString(16)).slice(-6)}getHSL(e,t=lt.workingColorSpace){lt.workingToColorSpace(Ot.copy(this),t);const n=Ot.r,s=Ot.g,r=Ot.b,a=Math.max(n,s,r),o=Math.min(n,s,r);let l,c;const u=(o+a)/2;if(o===a)l=0,c=0;else{const d=a-o;switch(c=u<=.5?d/(a+o):d/(2-a-o),a){case n:l=(s-r)/d+(s<r?6:0);break;case s:l=(r-n)/d+2;break;case r:l=(n-s)/d+4;break}l/=6}return e.h=l,e.s=c,e.l=u,e}getRGB(e,t=lt.workingColorSpace){return lt.workingToColorSpace(Ot.copy(this),t),e.r=Ot.r,e.g=Ot.g,e.b=Ot.b,e}getStyle(e=rn){lt.workingToColorSpace(Ot.copy(this),e);const t=Ot.r,n=Ot.g,s=Ot.b;return e!==rn?`color(${e} ${t.toFixed(3)} ${n.toFixed(3)} ${s.toFixed(3)})`:`rgb(${Math.round(t*255)},${Math.round(n*255)},${Math.round(s*255)})`}offsetHSL(e,t,n){return this.getHSL(zn),this.setHSL(zn.h+e,zn.s+t,zn.l+n)}add(e){return this.r+=e.r,this.g+=e.g,this.b+=e.b,this}addColors(e,t){return this.r=e.r+t.r,this.g=e.g+t.g,this.b=e.b+t.b,this}addScalar(e){return this.r+=e,this.g+=e,this.b+=e,this}sub(e){return this.r=Math.max(0,this.r-e.r),this.g=Math.max(0,this.g-e.g),this.b=Math.max(0,this.b-e.b),this}multiply(e){return this.r*=e.r,this.g*=e.g,this.b*=e.b,this}multiplyScalar(e){return this.r*=e,this.g*=e,this.b*=e,this}lerp(e,t){return this.r+=(e.r-this.r)*t,this.g+=(e.g-this.g)*t,this.b+=(e.b-this.b)*t,this}lerpColors(e,t,n){return this.r=e.r+(t.r-e.r)*n,this.g=e.g+(t.g-e.g)*n,this.b=e.b+(t.b-e.b)*n,this}lerpHSL(e,t){this.getHSL(zn),e.getHSL(Ps);const n=ss(zn.h,Ps.h,t),s=ss(zn.s,Ps.s,t),r=ss(zn.l,Ps.l,t);return this.setHSL(n,s,r),this}setFromVector3(e){return this.r=e.x,this.g=e.y,this.b=e.z,this}applyMatrix3(e){const t=this.r,n=this.g,s=this.b,r=e.elements;return this.r=r[0]*t+r[3]*n+r[6]*s,this.g=r[1]*t+r[4]*n+r[7]*s,this.b=r[2]*t+r[5]*n+r[8]*s,this}equals(e){return e.r===this.r&&e.g===this.g&&e.b===this.b}fromArray(e,t=0){return this.r=e[t],this.g=e[t+1],this.b=e[t+2],this}toArray(e=[],t=0){return e[t]=this.r,e[t+1]=this.g,e[t+2]=this.b,e}fromBufferAttribute(e,t){return this.r=e.getX(t),this.g=e.getY(t),this.b=e.getZ(t),this}toJSON(){return this.getHex()}*[Symbol.iterator](){yield this.r,yield this.g,yield this.b}}const Ot=new Le;Le.NAMES=Ic;let Sf=0;class pi extends Wi{constructor(){super(),this.isMaterial=!0,Object.defineProperty(this,"id",{value:Sf++}),this.uuid=$i(),this.name="",this.type="Material",this.blending=ki,this.side=qn,this.vertexColors=!1,this.opacity=1,this.transparent=!1,this.alphaHash=!1,this.blendSrc=va,this.blendDst=xa,this.blendEquation=ai,this.blendSrcAlpha=null,this.blendDstAlpha=null,this.blendEquationAlpha=null,this.blendColor=new Le(0,0,0),this.blendAlpha=0,this.depthFunc=Bi,this.depthTest=!0,this.depthWrite=!0,this.stencilWriteMask=255,this.stencilFunc=il,this.stencilRef=0,this.stencilFuncMask=255,this.stencilFail=_i,this.stencilZFail=_i,this.stencilZPass=_i,this.stencilWrite=!1,this.clippingPlanes=null,this.clipIntersection=!1,this.clipShadows=!1,this.shadowSide=null,this.colorWrite=!0,this.precision=null,this.polygonOffset=!1,this.polygonOffsetFactor=0,this.polygonOffsetUnits=0,this.dithering=!1,this.alphaToCoverage=!1,this.premultipliedAlpha=!1,this.forceSinglePass=!1,this.allowOverride=!0,this.visible=!0,this.toneMapped=!0,this.userData={},this.version=0,this._alphaTest=0}get alphaTest(){return this._alphaTest}set alphaTest(e){this._alphaTest>0!=e>0&&this.version++,this._alphaTest=e}onBeforeRender(){}onBeforeCompile(){}customProgramCacheKey(){return this.onBeforeCompile.toString()}setValues(e){if(e!==void 0)for(const t in e){const n=e[t];if(n===void 0){console.warn(`THREE.Material: parameter '${t}' has value of undefined.`);continue}const s=this[t];if(s===void 0){console.warn(`THREE.Material: '${t}' is not a property of THREE.${this.type}.`);continue}s&&s.isColor?s.set(n):s&&s.isVector3&&n&&n.isVector3?s.copy(n):this[t]=n}}toJSON(e){const t=e===void 0||typeof e=="string";t&&(e={textures:{},images:{}});const n={metadata:{version:4.7,type:"Material",generator:"Material.toJSON"}};n.uuid=this.uuid,n.type=this.type,this.name!==""&&(n.name=this.name),this.color&&this.color.isColor&&(n.color=this.color.getHex()),this.roughness!==void 0&&(n.roughness=this.roughness),this.metalness!==void 0&&(n.metalness=this.metalness),this.sheen!==void 0&&(n.sheen=this.sheen),this.sheenColor&&this.sheenColor.isColor&&(n.sheenColor=this.sheenColor.getHex()),this.sheenRoughness!==void 0&&(n.sheenRoughness=this.sheenRoughness),this.emissive&&this.emissive.isColor&&(n.emissive=this.emissive.getHex()),this.emissiveIntensity!==void 0&&this.emissiveIntensity!==1&&(n.emissiveIntensity=this.emissiveIntensity),this.specular&&this.specular.isColor&&(n.specular=this.specular.getHex()),this.specularIntensity!==void 0&&(n.specularIntensity=this.specularIntensity),this.specularColor&&this.specularColor.isColor&&(n.specularColor=this.specularColor.getHex()),this.shininess!==void 0&&(n.shininess=this.shininess),this.clearcoat!==void 0&&(n.clearcoat=this.clearcoat),this.clearcoatRoughness!==void 0&&(n.clearcoatRoughness=this.clearcoatRoughness),this.clearcoatMap&&this.clearcoatMap.isTexture&&(n.clearcoatMap=this.clearcoatMap.toJSON(e).uuid),this.clearcoatRoughnessMap&&this.clearcoatRoughnessMap.isTexture&&(n.clearcoatRoughnessMap=this.clearcoatRoughnessMap.toJSON(e).uuid),this.clearcoatNormalMap&&this.clearcoatNormalMap.isTexture&&(n.clearcoatNormalMap=this.clearcoatNormalMap.toJSON(e).uuid,n.clearcoatNormalScale=this.clearcoatNormalScale.toArray()),this.dispersion!==void 0&&(n.dispersion=this.dispersion),this.iridescence!==void 0&&(n.iridescence=this.iridescence),this.iridescenceIOR!==void 0&&(n.iridescenceIOR=this.iridescenceIOR),this.iridescenceThicknessRange!==void 0&&(n.iridescenceThicknessRange=this.iridescenceThicknessRange),this.iridescenceMap&&this.iridescenceMap.isTexture&&(n.iridescenceMap=this.iridescenceMap.toJSON(e).uuid),this.iridescenceThicknessMap&&this.iridescenceThicknessMap.isTexture&&(n.iridescenceThicknessMap=this.iridescenceThicknessMap.toJSON(e).uuid),this.anisotropy!==void 0&&(n.anisotropy=this.anisotropy),this.anisotropyRotation!==void 0&&(n.anisotropyRotation=this.anisotropyRotation),this.anisotropyMap&&this.anisotropyMap.isTexture&&(n.anisotropyMap=this.anisotropyMap.toJSON(e).uuid),this.map&&this.map.isTexture&&(n.map=this.map.toJSON(e).uuid),this.matcap&&this.matcap.isTexture&&(n.matcap=this.matcap.toJSON(e).uuid),this.alphaMap&&this.alphaMap.isTexture&&(n.alphaMap=this.alphaMap.toJSON(e).uuid),this.lightMap&&this.lightMap.isTexture&&(n.lightMap=this.lightMap.toJSON(e).uuid,n.lightMapIntensity=this.lightMapIntensity),this.aoMap&&this.aoMap.isTexture&&(n.aoMap=this.aoMap.toJSON(e).uuid,n.aoMapIntensity=this.aoMapIntensity),this.bumpMap&&this.bumpMap.isTexture&&(n.bumpMap=this.bumpMap.toJSON(e).uuid,n.bumpScale=this.bumpScale),this.normalMap&&this.normalMap.isTexture&&(n.normalMap=this.normalMap.toJSON(e).uuid,n.normalMapType=this.normalMapType,n.normalScale=this.normalScale.toArray()),this.displacementMap&&this.displacementMap.isTexture&&(n.displacementMap=this.displacementMap.toJSON(e).uuid,n.displacementScale=this.displacementScale,n.displacementBias=this.displacementBias),this.roughnessMap&&this.roughnessMap.isTexture&&(n.roughnessMap=this.roughnessMap.toJSON(e).uuid),this.metalnessMap&&this.metalnessMap.isTexture&&(n.metalnessMap=this.metalnessMap.toJSON(e).uuid),this.emissiveMap&&this.emissiveMap.isTexture&&(n.emissiveMap=this.emissiveMap.toJSON(e).uuid),this.specularMap&&this.specularMap.isTexture&&(n.specularMap=this.specularMap.toJSON(e).uuid),this.specularIntensityMap&&this.specularIntensityMap.isTexture&&(n.specularIntensityMap=this.specularIntensityMap.toJSON(e).uuid),this.specularColorMap&&this.specularColorMap.isTexture&&(n.specularColorMap=this.specularColorMap.toJSON(e).uuid),this.envMap&&this.envMap.isTexture&&(n.envMap=this.envMap.toJSON(e).uuid,this.combine!==void 0&&(n.combine=this.combine)),this.envMapRotation!==void 0&&(n.envMapRotation=this.envMapRotation.toArray()),this.envMapIntensity!==void 0&&(n.envMapIntensity=this.envMapIntensity),this.reflectivity!==void 0&&(n.reflectivity=this.reflectivity),this.refractionRatio!==void 0&&(n.refractionRatio=this.refractionRatio),this.gradientMap&&this.gradientMap.isTexture&&(n.gradientMap=this.gradientMap.toJSON(e).uuid),this.transmission!==void 0&&(n.transmission=this.transmission),this.transmissionMap&&this.transmissionMap.isTexture&&(n.transmissionMap=this.transmissionMap.toJSON(e).uuid),this.thickness!==void 0&&(n.thickness=this.thickness),this.thicknessMap&&this.thicknessMap.isTexture&&(n.thicknessMap=this.thicknessMap.toJSON(e).uuid),this.attenuationDistance!==void 0&&this.attenuationDistance!==1/0&&(n.attenuationDistance=this.attenuationDistance),this.attenuationColor!==void 0&&(n.attenuationColor=this.attenuationColor.getHex()),this.size!==void 0&&(n.size=this.size),this.shadowSide!==null&&(n.shadowSide=this.shadowSide),this.sizeAttenuation!==void 0&&(n.sizeAttenuation=this.sizeAttenuation),this.blending!==ki&&(n.blending=this.blending),this.side!==qn&&(n.side=this.side),this.vertexColors===!0&&(n.vertexColors=!0),this.opacity<1&&(n.opacity=this.opacity),this.transparent===!0&&(n.transparent=!0),this.blendSrc!==va&&(n.blendSrc=this.blendSrc),this.blendDst!==xa&&(n.blendDst=this.blendDst),this.blendEquation!==ai&&(n.blendEquation=this.blendEquation),this.blendSrcAlpha!==null&&(n.blendSrcAlpha=this.blendSrcAlpha),this.blendDstAlpha!==null&&(n.blendDstAlpha=this.blendDstAlpha),this.blendEquationAlpha!==null&&(n.blendEquationAlpha=this.blendEquationAlpha),this.blendColor&&this.blendColor.isColor&&(n.blendColor=this.blendColor.getHex()),this.blendAlpha!==0&&(n.blendAlpha=this.blendAlpha),this.depthFunc!==Bi&&(n.depthFunc=this.depthFunc),this.depthTest===!1&&(n.depthTest=this.depthTest),this.depthWrite===!1&&(n.depthWrite=this.depthWrite),this.colorWrite===!1&&(n.colorWrite=this.colorWrite),this.stencilWriteMask!==255&&(n.stencilWriteMask=this.stencilWriteMask),this.stencilFunc!==il&&(n.stencilFunc=this.stencilFunc),this.stencilRef!==0&&(n.stencilRef=this.stencilRef),this.stencilFuncMask!==255&&(n.stencilFuncMask=this.stencilFuncMask),this.stencilFail!==_i&&(n.stencilFail=this.stencilFail),this.stencilZFail!==_i&&(n.stencilZFail=this.stencilZFail),this.stencilZPass!==_i&&(n.stencilZPass=this.stencilZPass),this.stencilWrite===!0&&(n.stencilWrite=this.stencilWrite),this.rotation!==void 0&&this.rotation!==0&&(n.rotation=this.rotation),this.polygonOffset===!0&&(n.polygonOffset=!0),this.polygonOffsetFactor!==0&&(n.polygonOffsetFactor=this.polygonOffsetFactor),this.polygonOffsetUnits!==0&&(n.polygonOffsetUnits=this.polygonOffsetUnits),this.linewidth!==void 0&&this.linewidth!==1&&(n.linewidth=this.linewidth),this.dashSize!==void 0&&(n.dashSize=this.dashSize),this.gapSize!==void 0&&(n.gapSize=this.gapSize),this.scale!==void 0&&(n.scale=this.scale),this.dithering===!0&&(n.dithering=!0),this.alphaTest>0&&(n.alphaTest=this.alphaTest),this.alphaHash===!0&&(n.alphaHash=!0),this.alphaToCoverage===!0&&(n.alphaToCoverage=!0),this.premultipliedAlpha===!0&&(n.premultipliedAlpha=!0),this.forceSinglePass===!0&&(n.forceSinglePass=!0),this.wireframe===!0&&(n.wireframe=!0),this.wireframeLinewidth>1&&(n.wireframeLinewidth=this.wireframeLinewidth),this.wireframeLinecap!=="round"&&(n.wireframeLinecap=this.wireframeLinecap),this.wireframeLinejoin!=="round"&&(n.wireframeLinejoin=this.wireframeLinejoin),this.flatShading===!0&&(n.flatShading=!0),this.visible===!1&&(n.visible=!1),this.toneMapped===!1&&(n.toneMapped=!1),this.fog===!1&&(n.fog=!1),Object.keys(this.userData).length>0&&(n.userData=this.userData);function s(r){const a=[];for(const o in r){const l=r[o];delete l.metadata,a.push(l)}return a}if(t){const r=s(e.textures),a=s(e.images);r.length>0&&(n.textures=r),a.length>0&&(n.images=a)}return n}clone(){return new this.constructor().copy(this)}copy(e){this.name=e.name,this.blending=e.blending,this.side=e.side,this.vertexColors=e.vertexColors,this.opacity=e.opacity,this.transparent=e.transparent,this.blendSrc=e.blendSrc,this.blendDst=e.blendDst,this.blendEquation=e.blendEquation,this.blendSrcAlpha=e.blendSrcAlpha,this.blendDstAlpha=e.blendDstAlpha,this.blendEquationAlpha=e.blendEquationAlpha,this.blendColor.copy(e.blendColor),this.blendAlpha=e.blendAlpha,this.depthFunc=e.depthFunc,this.depthTest=e.depthTest,this.depthWrite=e.depthWrite,this.stencilWriteMask=e.stencilWriteMask,this.stencilFunc=e.stencilFunc,this.stencilRef=e.stencilRef,this.stencilFuncMask=e.stencilFuncMask,this.stencilFail=e.stencilFail,this.stencilZFail=e.stencilZFail,this.stencilZPass=e.stencilZPass,this.stencilWrite=e.stencilWrite;const t=e.clippingPlanes;let n=null;if(t!==null){const s=t.length;n=new Array(s);for(let r=0;r!==s;++r)n[r]=t[r].clone()}return this.clippingPlanes=n,this.clipIntersection=e.clipIntersection,this.clipShadows=e.clipShadows,this.shadowSide=e.shadowSide,this.colorWrite=e.colorWrite,this.precision=e.precision,this.polygonOffset=e.polygonOffset,this.polygonOffsetFactor=e.polygonOffsetFactor,this.polygonOffsetUnits=e.polygonOffsetUnits,this.dithering=e.dithering,this.alphaTest=e.alphaTest,this.alphaHash=e.alphaHash,this.alphaToCoverage=e.alphaToCoverage,this.premultipliedAlpha=e.premultipliedAlpha,this.forceSinglePass=e.forceSinglePass,this.visible=e.visible,this.toneMapped=e.toneMapped,this.userData=JSON.parse(JSON.stringify(e.userData)),this}dispose(){this.dispatchEvent({type:"dispose"})}set needsUpdate(e){e===!0&&this.version++}}class hs extends pi{constructor(e){super(),this.isMeshBasicMaterial=!0,this.type="MeshBasicMaterial",this.color=new Le(16777215),this.map=null,this.lightMap=null,this.lightMapIntensity=1,this.aoMap=null,this.aoMapIntensity=1,this.specularMap=null,this.alphaMap=null,this.envMap=null,this.envMapRotation=new Sn,this.combine=vo,this.reflectivity=1,this.refractionRatio=.98,this.wireframe=!1,this.wireframeLinewidth=1,this.wireframeLinecap="round",this.wireframeLinejoin="round",this.fog=!0,this.setValues(e)}copy(e){return super.copy(e),this.color.copy(e.color),this.map=e.map,this.lightMap=e.lightMap,this.lightMapIntensity=e.lightMapIntensity,this.aoMap=e.aoMap,this.aoMapIntensity=e.aoMapIntensity,this.specularMap=e.specularMap,this.alphaMap=e.alphaMap,this.envMap=e.envMap,this.envMapRotation.copy(e.envMapRotation),this.combine=e.combine,this.reflectivity=e.reflectivity,this.refractionRatio=e.refractionRatio,this.wireframe=e.wireframe,this.wireframeLinewidth=e.wireframeLinewidth,this.wireframeLinecap=e.wireframeLinecap,this.wireframeLinejoin=e.wireframeLinejoin,this.fog=e.fog,this}}const Ct=new O,Ls=new et;let yf=0;class en{constructor(e,t,n=!1){if(Array.isArray(e))throw new TypeError("THREE.BufferAttribute: array should be a Typed Array.");this.isBufferAttribute=!0,Object.defineProperty(this,"id",{value:yf++}),this.name="",this.array=e,this.itemSize=t,this.count=e!==void 0?e.length/t:0,this.normalized=n,this.usage=sl,this.updateRanges=[],this.gpuType=Pn,this.version=0}onUploadCallback(){}set needsUpdate(e){e===!0&&this.version++}setUsage(e){return this.usage=e,this}addUpdateRange(e,t){this.updateRanges.push({start:e,count:t})}clearUpdateRanges(){this.updateRanges.length=0}copy(e){return this.name=e.name,this.array=new e.array.constructor(e.array),this.itemSize=e.itemSize,this.count=e.count,this.normalized=e.normalized,this.usage=e.usage,this.gpuType=e.gpuType,this}copyAt(e,t,n){e*=this.itemSize,n*=t.itemSize;for(let s=0,r=this.itemSize;s<r;s++)this.array[e+s]=t.array[n+s];return this}copyArray(e){return this.array.set(e),this}applyMatrix3(e){if(this.itemSize===2)for(let t=0,n=this.count;t<n;t++)Ls.fromBufferAttribute(this,t),Ls.applyMatrix3(e),this.setXY(t,Ls.x,Ls.y);else if(this.itemSize===3)for(let t=0,n=this.count;t<n;t++)Ct.fromBufferAttribute(this,t),Ct.applyMatrix3(e),this.setXYZ(t,Ct.x,Ct.y,Ct.z);return this}applyMatrix4(e){for(let t=0,n=this.count;t<n;t++)Ct.fromBufferAttribute(this,t),Ct.applyMatrix4(e),this.setXYZ(t,Ct.x,Ct.y,Ct.z);return this}applyNormalMatrix(e){for(let t=0,n=this.count;t<n;t++)Ct.fromBufferAttribute(this,t),Ct.applyNormalMatrix(e),this.setXYZ(t,Ct.x,Ct.y,Ct.z);return this}transformDirection(e){for(let t=0,n=this.count;t<n;t++)Ct.fromBufferAttribute(this,t),Ct.transformDirection(e),this.setXYZ(t,Ct.x,Ct.y,Ct.z);return this}set(e,t=0){return this.array.set(e,t),this}getComponent(e,t){let n=this.array[e*this.itemSize+t];return this.normalized&&(n=Di(n,this.array)),n}setComponent(e,t,n){return this.normalized&&(n=Ht(n,this.array)),this.array[e*this.itemSize+t]=n,this}getX(e){let t=this.array[e*this.itemSize];return this.normalized&&(t=Di(t,this.array)),t}setX(e,t){return this.normalized&&(t=Ht(t,this.array)),this.array[e*this.itemSize]=t,this}getY(e){let t=this.array[e*this.itemSize+1];return this.normalized&&(t=Di(t,this.array)),t}setY(e,t){return this.normalized&&(t=Ht(t,this.array)),this.array[e*this.itemSize+1]=t,this}getZ(e){let t=this.array[e*this.itemSize+2];return this.normalized&&(t=Di(t,this.array)),t}setZ(e,t){return this.normalized&&(t=Ht(t,this.array)),this.array[e*this.itemSize+2]=t,this}getW(e){let t=this.array[e*this.itemSize+3];return this.normalized&&(t=Di(t,this.array)),t}setW(e,t){return this.normalized&&(t=Ht(t,this.array)),this.array[e*this.itemSize+3]=t,this}setXY(e,t,n){return e*=this.itemSize,this.normalized&&(t=Ht(t,this.array),n=Ht(n,this.array)),this.array[e+0]=t,this.array[e+1]=n,this}setXYZ(e,t,n,s){return e*=this.itemSize,this.normalized&&(t=Ht(t,this.array),n=Ht(n,this.array),s=Ht(s,this.array)),this.array[e+0]=t,this.array[e+1]=n,this.array[e+2]=s,this}setXYZW(e,t,n,s,r){return e*=this.itemSize,this.normalized&&(t=Ht(t,this.array),n=Ht(n,this.array),s=Ht(s,this.array),r=Ht(r,this.array)),this.array[e+0]=t,this.array[e+1]=n,this.array[e+2]=s,this.array[e+3]=r,this}onUpload(e){return this.onUploadCallback=e,this}clone(){return new this.constructor(this.array,this.itemSize).copy(this)}toJSON(){const e={itemSize:this.itemSize,type:this.array.constructor.name,array:Array.from(this.array),normalized:this.normalized};return this.name!==""&&(e.name=this.name),this.usage!==sl&&(e.usage=this.usage),e}}class Uc extends en{constructor(e,t,n){super(new Uint16Array(e),t,n)}}class kc extends en{constructor(e,t,n){super(new Uint32Array(e),t,n)}}class Nt extends en{constructor(e,t,n){super(new Float32Array(e),t,n)}}let bf=0;const sn=new bt,Yr=new Ut,Ai=new O,Qt=new gs,Ki=new gs,It=new O;class jt extends Wi{constructor(){super(),this.isBufferGeometry=!0,Object.defineProperty(this,"id",{value:bf++}),this.uuid=$i(),this.name="",this.type="BufferGeometry",this.index=null,this.indirect=null,this.attributes={},this.morphAttributes={},this.morphTargetsRelative=!1,this.groups=[],this.boundingBox=null,this.boundingSphere=null,this.drawRange={start:0,count:1/0},this.userData={}}getIndex(){return this.index}setIndex(e){return Array.isArray(e)?this.index=new(Lc(e)?kc:Uc)(e,1):this.index=e,this}setIndirect(e){return this.indirect=e,this}getIndirect(){return this.indirect}getAttribute(e){return this.attributes[e]}setAttribute(e,t){return this.attributes[e]=t,this}deleteAttribute(e){return delete this.attributes[e],this}hasAttribute(e){return this.attributes[e]!==void 0}addGroup(e,t,n=0){this.groups.push({start:e,count:t,materialIndex:n})}clearGroups(){this.groups=[]}setDrawRange(e,t){this.drawRange.start=e,this.drawRange.count=t}applyMatrix4(e){const t=this.attributes.position;t!==void 0&&(t.applyMatrix4(e),t.needsUpdate=!0);const n=this.attributes.normal;if(n!==void 0){const r=new Ze().getNormalMatrix(e);n.applyNormalMatrix(r),n.needsUpdate=!0}const s=this.attributes.tangent;return s!==void 0&&(s.transformDirection(e),s.needsUpdate=!0),this.boundingBox!==null&&this.computeBoundingBox(),this.boundingSphere!==null&&this.computeBoundingSphere(),this}applyQuaternion(e){return sn.makeRotationFromQuaternion(e),this.applyMatrix4(sn),this}rotateX(e){return sn.makeRotationX(e),this.applyMatrix4(sn),this}rotateY(e){return sn.makeRotationY(e),this.applyMatrix4(sn),this}rotateZ(e){return sn.makeRotationZ(e),this.applyMatrix4(sn),this}translate(e,t,n){return sn.makeTranslation(e,t,n),this.applyMatrix4(sn),this}scale(e,t,n){return sn.makeScale(e,t,n),this.applyMatrix4(sn),this}lookAt(e){return Yr.lookAt(e),Yr.updateMatrix(),this.applyMatrix4(Yr.matrix),this}center(){return this.computeBoundingBox(),this.boundingBox.getCenter(Ai).negate(),this.translate(Ai.x,Ai.y,Ai.z),this}setFromPoints(e){const t=this.getAttribute("position");if(t===void 0){const n=[];for(let s=0,r=e.length;s<r;s++){const a=e[s];n.push(a.x,a.y,a.z||0)}this.setAttribute("position",new Nt(n,3))}else{const n=Math.min(e.length,t.count);for(let s=0;s<n;s++){const r=e[s];t.setXYZ(s,r.x,r.y,r.z||0)}e.length>t.count&&console.warn("THREE.BufferGeometry: Buffer size too small for points data. Use .dispose() and create a new geometry."),t.needsUpdate=!0}return this}computeBoundingBox(){this.boundingBox===null&&(this.boundingBox=new gs);const e=this.attributes.position,t=this.morphAttributes.position;if(e&&e.isGLBufferAttribute){console.error("THREE.BufferGeometry.computeBoundingBox(): GLBufferAttribute requires a manual bounding box.",this),this.boundingBox.set(new O(-1/0,-1/0,-1/0),new O(1/0,1/0,1/0));return}if(e!==void 0){if(this.boundingBox.setFromBufferAttribute(e),t)for(let n=0,s=t.length;n<s;n++){const r=t[n];Qt.setFromBufferAttribute(r),this.morphTargetsRelative?(It.addVectors(this.boundingBox.min,Qt.min),this.boundingBox.expandByPoint(It),It.addVectors(this.boundingBox.max,Qt.max),this.boundingBox.expandByPoint(It)):(this.boundingBox.expandByPoint(Qt.min),this.boundingBox.expandByPoint(Qt.max))}}else this.boundingBox.makeEmpty();(isNaN(this.boundingBox.min.x)||isNaN(this.boundingBox.min.y)||isNaN(this.boundingBox.min.z))&&console.error('THREE.BufferGeometry.computeBoundingBox(): Computed min/max have NaN values. The "position" attribute is likely to have NaN values.',this)}computeBoundingSphere(){this.boundingSphere===null&&(this.boundingSphere=new _s);const e=this.attributes.position,t=this.morphAttributes.position;if(e&&e.isGLBufferAttribute){console.error("THREE.BufferGeometry.computeBoundingSphere(): GLBufferAttribute requires a manual bounding sphere.",this),this.boundingSphere.set(new O,1/0);return}if(e){const n=this.boundingSphere.center;if(Qt.setFromBufferAttribute(e),t)for(let r=0,a=t.length;r<a;r++){const o=t[r];Ki.setFromBufferAttribute(o),this.morphTargetsRelative?(It.addVectors(Qt.min,Ki.min),Qt.expandByPoint(It),It.addVectors(Qt.max,Ki.max),Qt.expandByPoint(It)):(Qt.expandByPoint(Ki.min),Qt.expandByPoint(Ki.max))}Qt.getCenter(n);let s=0;for(let r=0,a=e.count;r<a;r++)It.fromBufferAttribute(e,r),s=Math.max(s,n.distanceToSquared(It));if(t)for(let r=0,a=t.length;r<a;r++){const o=t[r],l=this.morphTargetsRelative;for(let c=0,u=o.count;c<u;c++)It.fromBufferAttribute(o,c),l&&(Ai.fromBufferAttribute(e,c),It.add(Ai)),s=Math.max(s,n.distanceToSquared(It))}this.boundingSphere.radius=Math.sqrt(s),isNaN(this.boundingSphere.radius)&&console.error('THREE.BufferGeometry.computeBoundingSphere(): Computed radius is NaN. The "position" attribute is likely to have NaN values.',this)}}computeTangents(){const e=this.index,t=this.attributes;if(e===null||t.position===void 0||t.normal===void 0||t.uv===void 0){console.error("THREE.BufferGeometry: .computeTangents() failed. Missing required attributes (index, position, normal or uv)");return}const n=t.position,s=t.normal,r=t.uv;this.hasAttribute("tangent")===!1&&this.setAttribute("tangent",new en(new Float32Array(4*n.count),4));const a=this.getAttribute("tangent"),o=[],l=[];for(let I=0;I<n.count;I++)o[I]=new O,l[I]=new O;const c=new O,u=new O,d=new O,h=new et,p=new et,g=new et,_=new O,m=new O;function f(I,y,S){c.fromBufferAttribute(n,I),u.fromBufferAttribute(n,y),d.fromBufferAttribute(n,S),h.fromBufferAttribute(r,I),p.fromBufferAttribute(r,y),g.fromBufferAttribute(r,S),u.sub(c),d.sub(c),p.sub(h),g.sub(h);const P=1/(p.x*g.y-g.x*p.y);isFinite(P)&&(_.copy(u).multiplyScalar(g.y).addScaledVector(d,-p.y).multiplyScalar(P),m.copy(d).multiplyScalar(p.x).addScaledVector(u,-g.x).multiplyScalar(P),o[I].add(_),o[y].add(_),o[S].add(_),l[I].add(m),l[y].add(m),l[S].add(m))}let E=this.groups;E.length===0&&(E=[{start:0,count:e.count}]);for(let I=0,y=E.length;I<y;++I){const S=E[I],P=S.start,$=S.count;for(let q=P,Y=P+$;q<Y;q+=3)f(e.getX(q+0),e.getX(q+1),e.getX(q+2))}const b=new O,v=new O,T=new O,A=new O;function C(I){T.fromBufferAttribute(s,I),A.copy(T);const y=o[I];b.copy(y),b.sub(T.multiplyScalar(T.dot(y))).normalize(),v.crossVectors(A,y);const P=v.dot(l[I])<0?-1:1;a.setXYZW(I,b.x,b.y,b.z,P)}for(let I=0,y=E.length;I<y;++I){const S=E[I],P=S.start,$=S.count;for(let q=P,Y=P+$;q<Y;q+=3)C(e.getX(q+0)),C(e.getX(q+1)),C(e.getX(q+2))}}computeVertexNormals(){const e=this.index,t=this.getAttribute("position");if(t!==void 0){let n=this.getAttribute("normal");if(n===void 0)n=new en(new Float32Array(t.count*3),3),this.setAttribute("normal",n);else for(let h=0,p=n.count;h<p;h++)n.setXYZ(h,0,0,0);const s=new O,r=new O,a=new O,o=new O,l=new O,c=new O,u=new O,d=new O;if(e)for(let h=0,p=e.count;h<p;h+=3){const g=e.getX(h+0),_=e.getX(h+1),m=e.getX(h+2);s.fromBufferAttribute(t,g),r.fromBufferAttribute(t,_),a.fromBufferAttribute(t,m),u.subVectors(a,r),d.subVectors(s,r),u.cross(d),o.fromBufferAttribute(n,g),l.fromBufferAttribute(n,_),c.fromBufferAttribute(n,m),o.add(u),l.add(u),c.add(u),n.setXYZ(g,o.x,o.y,o.z),n.setXYZ(_,l.x,l.y,l.z),n.setXYZ(m,c.x,c.y,c.z)}else for(let h=0,p=t.count;h<p;h+=3)s.fromBufferAttribute(t,h+0),r.fromBufferAttribute(t,h+1),a.fromBufferAttribute(t,h+2),u.subVectors(a,r),d.subVectors(s,r),u.cross(d),n.setXYZ(h+0,u.x,u.y,u.z),n.setXYZ(h+1,u.x,u.y,u.z),n.setXYZ(h+2,u.x,u.y,u.z);this.normalizeNormals(),n.needsUpdate=!0}}normalizeNormals(){const e=this.attributes.normal;for(let t=0,n=e.count;t<n;t++)It.fromBufferAttribute(e,t),It.normalize(),e.setXYZ(t,It.x,It.y,It.z)}toNonIndexed(){function e(o,l){const c=o.array,u=o.itemSize,d=o.normalized,h=new c.constructor(l.length*u);let p=0,g=0;for(let _=0,m=l.length;_<m;_++){o.isInterleavedBufferAttribute?p=l[_]*o.data.stride+o.offset:p=l[_]*u;for(let f=0;f<u;f++)h[g++]=c[p++]}return new en(h,u,d)}if(this.index===null)return console.warn("THREE.BufferGeometry.toNonIndexed(): BufferGeometry is already non-indexed."),this;const t=new jt,n=this.index.array,s=this.attributes;for(const o in s){const l=s[o],c=e(l,n);t.setAttribute(o,c)}const r=this.morphAttributes;for(const o in r){const l=[],c=r[o];for(let u=0,d=c.length;u<d;u++){const h=c[u],p=e(h,n);l.push(p)}t.morphAttributes[o]=l}t.morphTargetsRelative=this.morphTargetsRelative;const a=this.groups;for(let o=0,l=a.length;o<l;o++){const c=a[o];t.addGroup(c.start,c.count,c.materialIndex)}return t}toJSON(){const e={metadata:{version:4.7,type:"BufferGeometry",generator:"BufferGeometry.toJSON"}};if(e.uuid=this.uuid,e.type=this.type,this.name!==""&&(e.name=this.name),Object.keys(this.userData).length>0&&(e.userData=this.userData),this.parameters!==void 0){const l=this.parameters;for(const c in l)l[c]!==void 0&&(e[c]=l[c]);return e}e.data={attributes:{}};const t=this.index;t!==null&&(e.data.index={type:t.array.constructor.name,array:Array.prototype.slice.call(t.array)});const n=this.attributes;for(const l in n){const c=n[l];e.data.attributes[l]=c.toJSON(e.data)}const s={};let r=!1;for(const l in this.morphAttributes){const c=this.morphAttributes[l],u=[];for(let d=0,h=c.length;d<h;d++){const p=c[d];u.push(p.toJSON(e.data))}u.length>0&&(s[l]=u,r=!0)}r&&(e.data.morphAttributes=s,e.data.morphTargetsRelative=this.morphTargetsRelative);const a=this.groups;a.length>0&&(e.data.groups=JSON.parse(JSON.stringify(a)));const o=this.boundingSphere;return o!==null&&(e.data.boundingSphere=o.toJSON()),e}clone(){return new this.constructor().copy(this)}copy(e){this.index=null,this.attributes={},this.morphAttributes={},this.groups=[],this.boundingBox=null,this.boundingSphere=null;const t={};this.name=e.name;const n=e.index;n!==null&&this.setIndex(n.clone());const s=e.attributes;for(const c in s){const u=s[c];this.setAttribute(c,u.clone(t))}const r=e.morphAttributes;for(const c in r){const u=[],d=r[c];for(let h=0,p=d.length;h<p;h++)u.push(d[h].clone(t));this.morphAttributes[c]=u}this.morphTargetsRelative=e.morphTargetsRelative;const a=e.groups;for(let c=0,u=a.length;c<u;c++){const d=a[c];this.addGroup(d.start,d.count,d.materialIndex)}const o=e.boundingBox;o!==null&&(this.boundingBox=o.clone());const l=e.boundingSphere;return l!==null&&(this.boundingSphere=l.clone()),this.drawRange.start=e.drawRange.start,this.drawRange.count=e.drawRange.count,this.userData=e.userData,this}dispose(){this.dispatchEvent({type:"dispose"})}}const xl=new bt,ei=new vr,Ds=new _s,Ml=new O,Is=new O,Us=new O,ks=new O,jr=new O,Ns=new O,Sl=new O,Fs=new O;class kt extends Ut{constructor(e=new jt,t=new hs){super(),this.isMesh=!0,this.type="Mesh",this.geometry=e,this.material=t,this.morphTargetDictionary=void 0,this.morphTargetInfluences=void 0,this.count=1,this.updateMorphTargets()}copy(e,t){return super.copy(e,t),e.morphTargetInfluences!==void 0&&(this.morphTargetInfluences=e.morphTargetInfluences.slice()),e.morphTargetDictionary!==void 0&&(this.morphTargetDictionary=Object.assign({},e.morphTargetDictionary)),this.material=Array.isArray(e.material)?e.material.slice():e.material,this.geometry=e.geometry,this}updateMorphTargets(){const t=this.geometry.morphAttributes,n=Object.keys(t);if(n.length>0){const s=t[n[0]];if(s!==void 0){this.morphTargetInfluences=[],this.morphTargetDictionary={};for(let r=0,a=s.length;r<a;r++){const o=s[r].name||String(r);this.morphTargetInfluences.push(0),this.morphTargetDictionary[o]=r}}}}getVertexPosition(e,t){const n=this.geometry,s=n.attributes.position,r=n.morphAttributes.position,a=n.morphTargetsRelative;t.fromBufferAttribute(s,e);const o=this.morphTargetInfluences;if(r&&o){Ns.set(0,0,0);for(let l=0,c=r.length;l<c;l++){const u=o[l],d=r[l];u!==0&&(jr.fromBufferAttribute(d,e),a?Ns.addScaledVector(jr,u):Ns.addScaledVector(jr.sub(t),u))}t.add(Ns)}return t}raycast(e,t){const n=this.geometry,s=this.material,r=this.matrixWorld;s!==void 0&&(n.boundingSphere===null&&n.computeBoundingSphere(),Ds.copy(n.boundingSphere),Ds.applyMatrix4(r),ei.copy(e.ray).recast(e.near),!(Ds.containsPoint(ei.origin)===!1&&(ei.intersectSphere(Ds,Ml)===null||ei.origin.distanceToSquared(Ml)>(e.far-e.near)**2))&&(xl.copy(r).invert(),ei.copy(e.ray).applyMatrix4(xl),!(n.boundingBox!==null&&ei.intersectsBox(n.boundingBox)===!1)&&this._computeIntersections(e,t,ei)))}_computeIntersections(e,t,n){let s;const r=this.geometry,a=this.material,o=r.index,l=r.attributes.position,c=r.attributes.uv,u=r.attributes.uv1,d=r.attributes.normal,h=r.groups,p=r.drawRange;if(o!==null)if(Array.isArray(a))for(let g=0,_=h.length;g<_;g++){const m=h[g],f=a[m.materialIndex],E=Math.max(m.start,p.start),b=Math.min(o.count,Math.min(m.start+m.count,p.start+p.count));for(let v=E,T=b;v<T;v+=3){const A=o.getX(v),C=o.getX(v+1),I=o.getX(v+2);s=Os(this,f,e,n,c,u,d,A,C,I),s&&(s.faceIndex=Math.floor(v/3),s.face.materialIndex=m.materialIndex,t.push(s))}}else{const g=Math.max(0,p.start),_=Math.min(o.count,p.start+p.count);for(let m=g,f=_;m<f;m+=3){const E=o.getX(m),b=o.getX(m+1),v=o.getX(m+2);s=Os(this,a,e,n,c,u,d,E,b,v),s&&(s.faceIndex=Math.floor(m/3),t.push(s))}}else if(l!==void 0)if(Array.isArray(a))for(let g=0,_=h.length;g<_;g++){const m=h[g],f=a[m.materialIndex],E=Math.max(m.start,p.start),b=Math.min(l.count,Math.min(m.start+m.count,p.start+p.count));for(let v=E,T=b;v<T;v+=3){const A=v,C=v+1,I=v+2;s=Os(this,f,e,n,c,u,d,A,C,I),s&&(s.faceIndex=Math.floor(v/3),s.face.materialIndex=m.materialIndex,t.push(s))}}else{const g=Math.max(0,p.start),_=Math.min(l.count,p.start+p.count);for(let m=g,f=_;m<f;m+=3){const E=m,b=m+1,v=m+2;s=Os(this,a,e,n,c,u,d,E,b,v),s&&(s.faceIndex=Math.floor(m/3),t.push(s))}}}}function Ef(i,e,t,n,s,r,a,o){let l;if(e.side===Yt?l=n.intersectTriangle(a,r,s,!0,o):l=n.intersectTriangle(s,r,a,e.side===qn,o),l===null)return null;Fs.copy(o),Fs.applyMatrix4(i.matrixWorld);const c=t.ray.origin.distanceTo(Fs);return c<t.near||c>t.far?null:{distance:c,point:Fs.clone(),object:i}}function Os(i,e,t,n,s,r,a,o,l,c){i.getVertexPosition(o,Is),i.getVertexPosition(l,Us),i.getVertexPosition(c,ks);const u=Ef(i,e,t,n,Is,Us,ks,Sl);if(u){const d=new O;dn.getBarycoord(Sl,Is,Us,ks,d),s&&(u.uv=dn.getInterpolatedAttribute(s,o,l,c,d,new et)),r&&(u.uv1=dn.getInterpolatedAttribute(r,o,l,c,d,new et)),a&&(u.normal=dn.getInterpolatedAttribute(a,o,l,c,d,new O),u.normal.dot(n.direction)>0&&u.normal.multiplyScalar(-1));const h={a:o,b:l,c,normal:new O,materialIndex:0};dn.getNormal(Is,Us,ks,h.normal),u.face=h,u.barycoord=d}return u}class vs extends jt{constructor(e=1,t=1,n=1,s=1,r=1,a=1){super(),this.type="BoxGeometry",this.parameters={width:e,height:t,depth:n,widthSegments:s,heightSegments:r,depthSegments:a};const o=this;s=Math.floor(s),r=Math.floor(r),a=Math.floor(a);const l=[],c=[],u=[],d=[];let h=0,p=0;g("z","y","x",-1,-1,n,t,e,a,r,0),g("z","y","x",1,-1,n,t,-e,a,r,1),g("x","z","y",1,1,e,n,t,s,a,2),g("x","z","y",1,-1,e,n,-t,s,a,3),g("x","y","z",1,-1,e,t,n,s,r,4),g("x","y","z",-1,-1,e,t,-n,s,r,5),this.setIndex(l),this.setAttribute("position",new Nt(c,3)),this.setAttribute("normal",new Nt(u,3)),this.setAttribute("uv",new Nt(d,2));function g(_,m,f,E,b,v,T,A,C,I,y){const S=v/C,P=T/I,$=v/2,q=T/2,Y=A/2,X=C+1,B=I+1;let Q=0,G=0;const ie=new O;for(let de=0;de<B;de++){const Ee=de*P-q;for(let Oe=0;Oe<X;Oe++){const at=Oe*S-$;ie[_]=at*E,ie[m]=Ee*b,ie[f]=Y,c.push(ie.x,ie.y,ie.z),ie[_]=0,ie[m]=0,ie[f]=A>0?1:-1,u.push(ie.x,ie.y,ie.z),d.push(Oe/C),d.push(1-de/I),Q+=1}}for(let de=0;de<I;de++)for(let Ee=0;Ee<C;Ee++){const Oe=h+Ee+X*de,at=h+Ee+X*(de+1),He=h+(Ee+1)+X*(de+1),j=h+(Ee+1)+X*de;l.push(Oe,at,j),l.push(at,He,j),G+=6}o.addGroup(p,G,y),p+=G,h+=Q}}copy(e){return super.copy(e),this.parameters=Object.assign({},e.parameters),this}static fromJSON(e){return new vs(e.width,e.height,e.depth,e.widthSegments,e.heightSegments,e.depthSegments)}}function Gi(i){const e={};for(const t in i){e[t]={};for(const n in i[t]){const s=i[t][n];s&&(s.isColor||s.isMatrix3||s.isMatrix4||s.isVector2||s.isVector3||s.isVector4||s.isTexture||s.isQuaternion)?s.isRenderTargetTexture?(console.warn("UniformsUtils: Textures of render targets cannot be cloned via cloneUniforms() or mergeUniforms()."),e[t][n]=null):e[t][n]=s.clone():Array.isArray(s)?e[t][n]=s.slice():e[t][n]=s}}return e}function Gt(i){const e={};for(let t=0;t<i.length;t++){const n=Gi(i[t]);for(const s in n)e[s]=n[s]}return e}function Tf(i){const e=[];for(let t=0;t<i.length;t++)e.push(i[t].clone());return e}function Nc(i){const e=i.getRenderTarget();return e===null?i.outputColorSpace:e.isXRRenderTarget===!0?e.texture.colorSpace:lt.workingColorSpace}const wf={clone:Gi,merge:Gt};var Af=`void main() {
	gl_Position = projectionMatrix * modelViewMatrix * vec4( position, 1.0 );
}`,Rf=`void main() {
	gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
}`;class on extends pi{constructor(e){super(),this.isShaderMaterial=!0,this.type="ShaderMaterial",this.defines={},this.uniforms={},this.uniformsGroups=[],this.vertexShader=Af,this.fragmentShader=Rf,this.linewidth=1,this.wireframe=!1,this.wireframeLinewidth=1,this.fog=!1,this.lights=!1,this.clipping=!1,this.forceSinglePass=!0,this.extensions={clipCullDistance:!1,multiDraw:!1},this.defaultAttributeValues={color:[1,1,1],uv:[0,0],uv1:[0,0]},this.index0AttributeName=void 0,this.uniformsNeedUpdate=!1,this.glslVersion=null,e!==void 0&&this.setValues(e)}copy(e){return super.copy(e),this.fragmentShader=e.fragmentShader,this.vertexShader=e.vertexShader,this.uniforms=Gi(e.uniforms),this.uniformsGroups=Tf(e.uniformsGroups),this.defines=Object.assign({},e.defines),this.wireframe=e.wireframe,this.wireframeLinewidth=e.wireframeLinewidth,this.fog=e.fog,this.lights=e.lights,this.clipping=e.clipping,this.extensions=Object.assign({},e.extensions),this.glslVersion=e.glslVersion,this}toJSON(e){const t=super.toJSON(e);t.glslVersion=this.glslVersion,t.uniforms={};for(const s in this.uniforms){const a=this.uniforms[s].value;a&&a.isTexture?t.uniforms[s]={type:"t",value:a.toJSON(e).uuid}:a&&a.isColor?t.uniforms[s]={type:"c",value:a.getHex()}:a&&a.isVector2?t.uniforms[s]={type:"v2",value:a.toArray()}:a&&a.isVector3?t.uniforms[s]={type:"v3",value:a.toArray()}:a&&a.isVector4?t.uniforms[s]={type:"v4",value:a.toArray()}:a&&a.isMatrix3?t.uniforms[s]={type:"m3",value:a.toArray()}:a&&a.isMatrix4?t.uniforms[s]={type:"m4",value:a.toArray()}:t.uniforms[s]={value:a}}Object.keys(this.defines).length>0&&(t.defines=this.defines),t.vertexShader=this.vertexShader,t.fragmentShader=this.fragmentShader,t.lights=this.lights,t.clipping=this.clipping;const n={};for(const s in this.extensions)this.extensions[s]===!0&&(n[s]=!0);return Object.keys(n).length>0&&(t.extensions=n),t}}class Fc extends Ut{constructor(){super(),this.isCamera=!0,this.type="Camera",this.matrixWorldInverse=new bt,this.projectionMatrix=new bt,this.projectionMatrixInverse=new bt,this.coordinateSystem=_n,this._reversedDepth=!1}get reversedDepth(){return this._reversedDepth}copy(e,t){return super.copy(e,t),this.matrixWorldInverse.copy(e.matrixWorldInverse),this.projectionMatrix.copy(e.projectionMatrix),this.projectionMatrixInverse.copy(e.projectionMatrixInverse),this.coordinateSystem=e.coordinateSystem,this}getWorldDirection(e){return super.getWorldDirection(e).negate()}updateMatrixWorld(e){super.updateMatrixWorld(e),this.matrixWorldInverse.copy(this.matrixWorld).invert()}updateWorldMatrix(e,t){super.updateWorldMatrix(e,t),this.matrixWorldInverse.copy(this.matrixWorld).invert()}clone(){return new this.constructor().copy(this)}}const Vn=new O,yl=new et,bl=new et;class Wt extends Fc{constructor(e=50,t=1,n=.1,s=2e3){super(),this.isPerspectiveCamera=!0,this.type="PerspectiveCamera",this.fov=e,this.zoom=1,this.near=n,this.far=s,this.focus=10,this.aspect=t,this.view=null,this.filmGauge=35,this.filmOffset=0,this.updateProjectionMatrix()}copy(e,t){return super.copy(e,t),this.fov=e.fov,this.zoom=e.zoom,this.near=e.near,this.far=e.far,this.focus=e.focus,this.aspect=e.aspect,this.view=e.view===null?null:Object.assign({},e.view),this.filmGauge=e.filmGauge,this.filmOffset=e.filmOffset,this}setFocalLength(e){const t=.5*this.getFilmHeight()/e;this.fov=us*2*Math.atan(t),this.updateProjectionMatrix()}getFocalLength(){const e=Math.tan(is*.5*this.fov);return .5*this.getFilmHeight()/e}getEffectiveFOV(){return us*2*Math.atan(Math.tan(is*.5*this.fov)/this.zoom)}getFilmWidth(){return this.filmGauge*Math.min(this.aspect,1)}getFilmHeight(){return this.filmGauge/Math.max(this.aspect,1)}getViewBounds(e,t,n){Vn.set(-1,-1,.5).applyMatrix4(this.projectionMatrixInverse),t.set(Vn.x,Vn.y).multiplyScalar(-e/Vn.z),Vn.set(1,1,.5).applyMatrix4(this.projectionMatrixInverse),n.set(Vn.x,Vn.y).multiplyScalar(-e/Vn.z)}getViewSize(e,t){return this.getViewBounds(e,yl,bl),t.subVectors(bl,yl)}setViewOffset(e,t,n,s,r,a){this.aspect=e/t,this.view===null&&(this.view={enabled:!0,fullWidth:1,fullHeight:1,offsetX:0,offsetY:0,width:1,height:1}),this.view.enabled=!0,this.view.fullWidth=e,this.view.fullHeight=t,this.view.offsetX=n,this.view.offsetY=s,this.view.width=r,this.view.height=a,this.updateProjectionMatrix()}clearViewOffset(){this.view!==null&&(this.view.enabled=!1),this.updateProjectionMatrix()}updateProjectionMatrix(){const e=this.near;let t=e*Math.tan(is*.5*this.fov)/this.zoom,n=2*t,s=this.aspect*n,r=-.5*s;const a=this.view;if(this.view!==null&&this.view.enabled){const l=a.fullWidth,c=a.fullHeight;r+=a.offsetX*s/l,t-=a.offsetY*n/c,s*=a.width/l,n*=a.height/c}const o=this.filmOffset;o!==0&&(r+=e*o/this.getFilmWidth()),this.projectionMatrix.makePerspective(r,r+s,t,t-n,e,this.far,this.coordinateSystem,this.reversedDepth),this.projectionMatrixInverse.copy(this.projectionMatrix).invert()}toJSON(e){const t=super.toJSON(e);return t.object.fov=this.fov,t.object.zoom=this.zoom,t.object.near=this.near,t.object.far=this.far,t.object.focus=this.focus,t.object.aspect=this.aspect,this.view!==null&&(t.object.view=Object.assign({},this.view)),t.object.filmGauge=this.filmGauge,t.object.filmOffset=this.filmOffset,t}}const Ri=-90,Ci=1;class Cf extends Ut{constructor(e,t,n){super(),this.type="CubeCamera",this.renderTarget=n,this.coordinateSystem=null,this.activeMipmapLevel=0;const s=new Wt(Ri,Ci,e,t);s.layers=this.layers,this.add(s);const r=new Wt(Ri,Ci,e,t);r.layers=this.layers,this.add(r);const a=new Wt(Ri,Ci,e,t);a.layers=this.layers,this.add(a);const o=new Wt(Ri,Ci,e,t);o.layers=this.layers,this.add(o);const l=new Wt(Ri,Ci,e,t);l.layers=this.layers,this.add(l);const c=new Wt(Ri,Ci,e,t);c.layers=this.layers,this.add(c)}updateCoordinateSystem(){const e=this.coordinateSystem,t=this.children.concat(),[n,s,r,a,o,l]=t;for(const c of t)this.remove(c);if(e===_n)n.up.set(0,1,0),n.lookAt(1,0,0),s.up.set(0,1,0),s.lookAt(-1,0,0),r.up.set(0,0,-1),r.lookAt(0,1,0),a.up.set(0,0,1),a.lookAt(0,-1,0),o.up.set(0,1,0),o.lookAt(0,0,1),l.up.set(0,1,0),l.lookAt(0,0,-1);else if(e===ar)n.up.set(0,-1,0),n.lookAt(-1,0,0),s.up.set(0,-1,0),s.lookAt(1,0,0),r.up.set(0,0,1),r.lookAt(0,1,0),a.up.set(0,0,-1),a.lookAt(0,-1,0),o.up.set(0,-1,0),o.lookAt(0,0,1),l.up.set(0,-1,0),l.lookAt(0,0,-1);else throw new Error("THREE.CubeCamera.updateCoordinateSystem(): Invalid coordinate system: "+e);for(const c of t)this.add(c),c.updateMatrixWorld()}update(e,t){this.parent===null&&this.updateMatrixWorld();const{renderTarget:n,activeMipmapLevel:s}=this;this.coordinateSystem!==e.coordinateSystem&&(this.coordinateSystem=e.coordinateSystem,this.updateCoordinateSystem());const[r,a,o,l,c,u]=this.children,d=e.getRenderTarget(),h=e.getActiveCubeFace(),p=e.getActiveMipmapLevel(),g=e.xr.enabled;e.xr.enabled=!1;const _=n.texture.generateMipmaps;n.texture.generateMipmaps=!1,e.setRenderTarget(n,0,s),e.render(t,r),e.setRenderTarget(n,1,s),e.render(t,a),e.setRenderTarget(n,2,s),e.render(t,o),e.setRenderTarget(n,3,s),e.render(t,l),e.setRenderTarget(n,4,s),e.render(t,c),n.texture.generateMipmaps=_,e.setRenderTarget(n,5,s),e.render(t,u),e.setRenderTarget(d,h,p),e.xr.enabled=g,n.texture.needsPMREMUpdate=!0}}class Oc extends Bt{constructor(e=[],t=zi,n,s,r,a,o,l,c,u){super(e,t,n,s,r,a,o,l,c,u),this.isCubeTexture=!0,this.flipY=!1}get images(){return this.image}set images(e){this.image=e}}class Pf extends di{constructor(e=1,t={}){super(e,e,t),this.isWebGLCubeRenderTarget=!0;const n={width:e,height:e,depth:1},s=[n,n,n,n,n,n];this.texture=new Oc(s),this._setTextureOptions(t),this.texture.isRenderTargetTexture=!0}fromEquirectangularTexture(e,t){this.texture.type=t.type,this.texture.colorSpace=t.colorSpace,this.texture.generateMipmaps=t.generateMipmaps,this.texture.minFilter=t.minFilter,this.texture.magFilter=t.magFilter;const n={uniforms:{tEquirect:{value:null}},vertexShader:`

				varying vec3 vWorldDirection;

				vec3 transformDirection( in vec3 dir, in mat4 matrix ) {

					return normalize( ( matrix * vec4( dir, 0.0 ) ).xyz );

				}

				void main() {

					vWorldDirection = transformDirection( position, modelMatrix );

					#include <begin_vertex>
					#include <project_vertex>

				}
			`,fragmentShader:`

				uniform sampler2D tEquirect;

				varying vec3 vWorldDirection;

				#include <common>

				void main() {

					vec3 direction = normalize( vWorldDirection );

					vec2 sampleUV = equirectUv( direction );

					gl_FragColor = texture2D( tEquirect, sampleUV );

				}
			`},s=new vs(5,5,5),r=new on({name:"CubemapFromEquirect",uniforms:Gi(n.uniforms),vertexShader:n.vertexShader,fragmentShader:n.fragmentShader,side:Yt,blending:Wn});r.uniforms.tEquirect.value=t;const a=new kt(s,r),o=t.minFilter;return t.minFilter===li&&(t.minFilter=an),new Cf(1,10,this).update(e,a),t.minFilter=o,a.geometry.dispose(),a.material.dispose(),this}clear(e,t=!0,n=!0,s=!0){const r=e.getRenderTarget();for(let a=0;a<6;a++)e.setRenderTarget(this,a),e.clear(t,n,s);e.setRenderTarget(r)}}class ts extends Ut{constructor(){super(),this.isGroup=!0,this.type="Group"}}const Lf={type:"move"};class Zr{constructor(){this._targetRay=null,this._grip=null,this._hand=null}getHandSpace(){return this._hand===null&&(this._hand=new ts,this._hand.matrixAutoUpdate=!1,this._hand.visible=!1,this._hand.joints={},this._hand.inputState={pinching:!1}),this._hand}getTargetRaySpace(){return this._targetRay===null&&(this._targetRay=new ts,this._targetRay.matrixAutoUpdate=!1,this._targetRay.visible=!1,this._targetRay.hasLinearVelocity=!1,this._targetRay.linearVelocity=new O,this._targetRay.hasAngularVelocity=!1,this._targetRay.angularVelocity=new O),this._targetRay}getGripSpace(){return this._grip===null&&(this._grip=new ts,this._grip.matrixAutoUpdate=!1,this._grip.visible=!1,this._grip.hasLinearVelocity=!1,this._grip.linearVelocity=new O,this._grip.hasAngularVelocity=!1,this._grip.angularVelocity=new O),this._grip}dispatchEvent(e){return this._targetRay!==null&&this._targetRay.dispatchEvent(e),this._grip!==null&&this._grip.dispatchEvent(e),this._hand!==null&&this._hand.dispatchEvent(e),this}connect(e){if(e&&e.hand){const t=this._hand;if(t)for(const n of e.hand.values())this._getHandJoint(t,n)}return this.dispatchEvent({type:"connected",data:e}),this}disconnect(e){return this.dispatchEvent({type:"disconnected",data:e}),this._targetRay!==null&&(this._targetRay.visible=!1),this._grip!==null&&(this._grip.visible=!1),this._hand!==null&&(this._hand.visible=!1),this}update(e,t,n){let s=null,r=null,a=null;const o=this._targetRay,l=this._grip,c=this._hand;if(e&&t.session.visibilityState!=="visible-blurred"){if(c&&e.hand){a=!0;for(const _ of e.hand.values()){const m=t.getJointPose(_,n),f=this._getHandJoint(c,_);m!==null&&(f.matrix.fromArray(m.transform.matrix),f.matrix.decompose(f.position,f.rotation,f.scale),f.matrixWorldNeedsUpdate=!0,f.jointRadius=m.radius),f.visible=m!==null}const u=c.joints["index-finger-tip"],d=c.joints["thumb-tip"],h=u.position.distanceTo(d.position),p=.02,g=.005;c.inputState.pinching&&h>p+g?(c.inputState.pinching=!1,this.dispatchEvent({type:"pinchend",handedness:e.handedness,target:this})):!c.inputState.pinching&&h<=p-g&&(c.inputState.pinching=!0,this.dispatchEvent({type:"pinchstart",handedness:e.handedness,target:this}))}else l!==null&&e.gripSpace&&(r=t.getPose(e.gripSpace,n),r!==null&&(l.matrix.fromArray(r.transform.matrix),l.matrix.decompose(l.position,l.rotation,l.scale),l.matrixWorldNeedsUpdate=!0,r.linearVelocity?(l.hasLinearVelocity=!0,l.linearVelocity.copy(r.linearVelocity)):l.hasLinearVelocity=!1,r.angularVelocity?(l.hasAngularVelocity=!0,l.angularVelocity.copy(r.angularVelocity)):l.hasAngularVelocity=!1));o!==null&&(s=t.getPose(e.targetRaySpace,n),s===null&&r!==null&&(s=r),s!==null&&(o.matrix.fromArray(s.transform.matrix),o.matrix.decompose(o.position,o.rotation,o.scale),o.matrixWorldNeedsUpdate=!0,s.linearVelocity?(o.hasLinearVelocity=!0,o.linearVelocity.copy(s.linearVelocity)):o.hasLinearVelocity=!1,s.angularVelocity?(o.hasAngularVelocity=!0,o.angularVelocity.copy(s.angularVelocity)):o.hasAngularVelocity=!1,this.dispatchEvent(Lf)))}return o!==null&&(o.visible=s!==null),l!==null&&(l.visible=r!==null),c!==null&&(c.visible=a!==null),this}_getHandJoint(e,t){if(e.joints[t.jointName]===void 0){const n=new ts;n.matrixAutoUpdate=!1,n.visible=!1,e.joints[t.jointName]=n,e.add(n)}return e.joints[t.jointName]}}class xr extends Ut{constructor(){super(),this.isScene=!0,this.type="Scene",this.background=null,this.environment=null,this.fog=null,this.backgroundBlurriness=0,this.backgroundIntensity=1,this.backgroundRotation=new Sn,this.environmentIntensity=1,this.environmentRotation=new Sn,this.overrideMaterial=null,typeof __THREE_DEVTOOLS__<"u"&&__THREE_DEVTOOLS__.dispatchEvent(new CustomEvent("observe",{detail:this}))}copy(e,t){return super.copy(e,t),e.background!==null&&(this.background=e.background.clone()),e.environment!==null&&(this.environment=e.environment.clone()),e.fog!==null&&(this.fog=e.fog.clone()),this.backgroundBlurriness=e.backgroundBlurriness,this.backgroundIntensity=e.backgroundIntensity,this.backgroundRotation.copy(e.backgroundRotation),this.environmentIntensity=e.environmentIntensity,this.environmentRotation.copy(e.environmentRotation),e.overrideMaterial!==null&&(this.overrideMaterial=e.overrideMaterial.clone()),this.matrixAutoUpdate=e.matrixAutoUpdate,this}toJSON(e){const t=super.toJSON(e);return this.fog!==null&&(t.object.fog=this.fog.toJSON()),this.backgroundBlurriness>0&&(t.object.backgroundBlurriness=this.backgroundBlurriness),this.backgroundIntensity!==1&&(t.object.backgroundIntensity=this.backgroundIntensity),t.object.backgroundRotation=this.backgroundRotation.toArray(),this.environmentIntensity!==1&&(t.object.environmentIntensity=this.environmentIntensity),t.object.environmentRotation=this.environmentRotation.toArray(),t}}const Kr=new O,Df=new O,If=new Ze;class si{constructor(e=new O(1,0,0),t=0){this.isPlane=!0,this.normal=e,this.constant=t}set(e,t){return this.normal.copy(e),this.constant=t,this}setComponents(e,t,n,s){return this.normal.set(e,t,n),this.constant=s,this}setFromNormalAndCoplanarPoint(e,t){return this.normal.copy(e),this.constant=-t.dot(this.normal),this}setFromCoplanarPoints(e,t,n){const s=Kr.subVectors(n,t).cross(Df.subVectors(e,t)).normalize();return this.setFromNormalAndCoplanarPoint(s,e),this}copy(e){return this.normal.copy(e.normal),this.constant=e.constant,this}normalize(){const e=1/this.normal.length();return this.normal.multiplyScalar(e),this.constant*=e,this}negate(){return this.constant*=-1,this.normal.negate(),this}distanceToPoint(e){return this.normal.dot(e)+this.constant}distanceToSphere(e){return this.distanceToPoint(e.center)-e.radius}projectPoint(e,t){return t.copy(e).addScaledVector(this.normal,-this.distanceToPoint(e))}intersectLine(e,t){const n=e.delta(Kr),s=this.normal.dot(n);if(s===0)return this.distanceToPoint(e.start)===0?t.copy(e.start):null;const r=-(e.start.dot(this.normal)+this.constant)/s;return r<0||r>1?null:t.copy(e.start).addScaledVector(n,r)}intersectsLine(e){const t=this.distanceToPoint(e.start),n=this.distanceToPoint(e.end);return t<0&&n>0||n<0&&t>0}intersectsBox(e){return e.intersectsPlane(this)}intersectsSphere(e){return e.intersectsPlane(this)}coplanarPoint(e){return e.copy(this.normal).multiplyScalar(-this.constant)}applyMatrix4(e,t){const n=t||If.getNormalMatrix(e),s=this.coplanarPoint(Kr).applyMatrix4(e),r=this.normal.applyMatrix3(n).normalize();return this.constant=-s.dot(r),this}translate(e){return this.constant-=e.dot(this.normal),this}equals(e){return e.normal.equals(this.normal)&&e.constant===this.constant}clone(){return new this.constructor().copy(this)}}const ti=new _s,Uf=new et(.5,.5),Bs=new O;class Ro{constructor(e=new si,t=new si,n=new si,s=new si,r=new si,a=new si){this.planes=[e,t,n,s,r,a]}set(e,t,n,s,r,a){const o=this.planes;return o[0].copy(e),o[1].copy(t),o[2].copy(n),o[3].copy(s),o[4].copy(r),o[5].copy(a),this}copy(e){const t=this.planes;for(let n=0;n<6;n++)t[n].copy(e.planes[n]);return this}setFromProjectionMatrix(e,t=_n,n=!1){const s=this.planes,r=e.elements,a=r[0],o=r[1],l=r[2],c=r[3],u=r[4],d=r[5],h=r[6],p=r[7],g=r[8],_=r[9],m=r[10],f=r[11],E=r[12],b=r[13],v=r[14],T=r[15];if(s[0].setComponents(c-a,p-u,f-g,T-E).normalize(),s[1].setComponents(c+a,p+u,f+g,T+E).normalize(),s[2].setComponents(c+o,p+d,f+_,T+b).normalize(),s[3].setComponents(c-o,p-d,f-_,T-b).normalize(),n)s[4].setComponents(l,h,m,v).normalize(),s[5].setComponents(c-l,p-h,f-m,T-v).normalize();else if(s[4].setComponents(c-l,p-h,f-m,T-v).normalize(),t===_n)s[5].setComponents(c+l,p+h,f+m,T+v).normalize();else if(t===ar)s[5].setComponents(l,h,m,v).normalize();else throw new Error("THREE.Frustum.setFromProjectionMatrix(): Invalid coordinate system: "+t);return this}intersectsObject(e){if(e.boundingSphere!==void 0)e.boundingSphere===null&&e.computeBoundingSphere(),ti.copy(e.boundingSphere).applyMatrix4(e.matrixWorld);else{const t=e.geometry;t.boundingSphere===null&&t.computeBoundingSphere(),ti.copy(t.boundingSphere).applyMatrix4(e.matrixWorld)}return this.intersectsSphere(ti)}intersectsSprite(e){ti.center.set(0,0,0);const t=Uf.distanceTo(e.center);return ti.radius=.7071067811865476+t,ti.applyMatrix4(e.matrixWorld),this.intersectsSphere(ti)}intersectsSphere(e){const t=this.planes,n=e.center,s=-e.radius;for(let r=0;r<6;r++)if(t[r].distanceToPoint(n)<s)return!1;return!0}intersectsBox(e){const t=this.planes;for(let n=0;n<6;n++){const s=t[n];if(Bs.x=s.normal.x>0?e.max.x:e.min.x,Bs.y=s.normal.y>0?e.max.y:e.min.y,Bs.z=s.normal.z>0?e.max.z:e.min.z,s.distanceToPoint(Bs)<0)return!1}return!0}containsPoint(e){const t=this.planes;for(let n=0;n<6;n++)if(t[n].distanceToPoint(e)<0)return!1;return!0}clone(){return new this.constructor().copy(this)}}class Bc extends pi{constructor(e){super(),this.isLineBasicMaterial=!0,this.type="LineBasicMaterial",this.color=new Le(16777215),this.map=null,this.linewidth=1,this.linecap="round",this.linejoin="round",this.fog=!0,this.setValues(e)}copy(e){return super.copy(e),this.color.copy(e.color),this.map=e.map,this.linewidth=e.linewidth,this.linecap=e.linecap,this.linejoin=e.linejoin,this.fog=e.fog,this}}const or=new O,lr=new O,El=new bt,Ji=new vr,zs=new _s,Jr=new O,Tl=new O;class kf extends Ut{constructor(e=new jt,t=new Bc){super(),this.isLine=!0,this.type="Line",this.geometry=e,this.material=t,this.morphTargetDictionary=void 0,this.morphTargetInfluences=void 0,this.updateMorphTargets()}copy(e,t){return super.copy(e,t),this.material=Array.isArray(e.material)?e.material.slice():e.material,this.geometry=e.geometry,this}computeLineDistances(){const e=this.geometry;if(e.index===null){const t=e.attributes.position,n=[0];for(let s=1,r=t.count;s<r;s++)or.fromBufferAttribute(t,s-1),lr.fromBufferAttribute(t,s),n[s]=n[s-1],n[s]+=or.distanceTo(lr);e.setAttribute("lineDistance",new Nt(n,1))}else console.warn("THREE.Line.computeLineDistances(): Computation only possible with non-indexed BufferGeometry.");return this}raycast(e,t){const n=this.geometry,s=this.matrixWorld,r=e.params.Line.threshold,a=n.drawRange;if(n.boundingSphere===null&&n.computeBoundingSphere(),zs.copy(n.boundingSphere),zs.applyMatrix4(s),zs.radius+=r,e.ray.intersectsSphere(zs)===!1)return;El.copy(s).invert(),Ji.copy(e.ray).applyMatrix4(El);const o=r/((this.scale.x+this.scale.y+this.scale.z)/3),l=o*o,c=this.isLineSegments?2:1,u=n.index,h=n.attributes.position;if(u!==null){const p=Math.max(0,a.start),g=Math.min(u.count,a.start+a.count);for(let _=p,m=g-1;_<m;_+=c){const f=u.getX(_),E=u.getX(_+1),b=Vs(this,e,Ji,l,f,E,_);b&&t.push(b)}if(this.isLineLoop){const _=u.getX(g-1),m=u.getX(p),f=Vs(this,e,Ji,l,_,m,g-1);f&&t.push(f)}}else{const p=Math.max(0,a.start),g=Math.min(h.count,a.start+a.count);for(let _=p,m=g-1;_<m;_+=c){const f=Vs(this,e,Ji,l,_,_+1,_);f&&t.push(f)}if(this.isLineLoop){const _=Vs(this,e,Ji,l,g-1,p,g-1);_&&t.push(_)}}}updateMorphTargets(){const t=this.geometry.morphAttributes,n=Object.keys(t);if(n.length>0){const s=t[n[0]];if(s!==void 0){this.morphTargetInfluences=[],this.morphTargetDictionary={};for(let r=0,a=s.length;r<a;r++){const o=s[r].name||String(r);this.morphTargetInfluences.push(0),this.morphTargetDictionary[o]=r}}}}}function Vs(i,e,t,n,s,r,a){const o=i.geometry.attributes.position;if(or.fromBufferAttribute(o,s),lr.fromBufferAttribute(o,r),t.distanceSqToSegment(or,lr,Jr,Tl)>n)return;Jr.applyMatrix4(i.matrixWorld);const c=e.ray.origin.distanceTo(Jr);if(!(c<e.near||c>e.far))return{distance:c,point:Tl.clone().applyMatrix4(i.matrixWorld),index:a,face:null,faceIndex:null,barycoord:null,object:i}}class zc extends pi{constructor(e){super(),this.isPointsMaterial=!0,this.type="PointsMaterial",this.color=new Le(16777215),this.map=null,this.alphaMap=null,this.size=1,this.sizeAttenuation=!0,this.fog=!0,this.setValues(e)}copy(e){return super.copy(e),this.color.copy(e.color),this.map=e.map,this.alphaMap=e.alphaMap,this.size=e.size,this.sizeAttenuation=e.sizeAttenuation,this.fog=e.fog,this}}const wl=new bt,io=new vr,Hs=new _s,Gs=new O;class Nf extends Ut{constructor(e=new jt,t=new zc){super(),this.isPoints=!0,this.type="Points",this.geometry=e,this.material=t,this.morphTargetDictionary=void 0,this.morphTargetInfluences=void 0,this.updateMorphTargets()}copy(e,t){return super.copy(e,t),this.material=Array.isArray(e.material)?e.material.slice():e.material,this.geometry=e.geometry,this}raycast(e,t){const n=this.geometry,s=this.matrixWorld,r=e.params.Points.threshold,a=n.drawRange;if(n.boundingSphere===null&&n.computeBoundingSphere(),Hs.copy(n.boundingSphere),Hs.applyMatrix4(s),Hs.radius+=r,e.ray.intersectsSphere(Hs)===!1)return;wl.copy(s).invert(),io.copy(e.ray).applyMatrix4(wl);const o=r/((this.scale.x+this.scale.y+this.scale.z)/3),l=o*o,c=n.index,d=n.attributes.position;if(c!==null){const h=Math.max(0,a.start),p=Math.min(c.count,a.start+a.count);for(let g=h,_=p;g<_;g++){const m=c.getX(g);Gs.fromBufferAttribute(d,m),Al(Gs,m,l,s,e,t,this)}}else{const h=Math.max(0,a.start),p=Math.min(d.count,a.start+a.count);for(let g=h,_=p;g<_;g++)Gs.fromBufferAttribute(d,g),Al(Gs,g,l,s,e,t,this)}}updateMorphTargets(){const t=this.geometry.morphAttributes,n=Object.keys(t);if(n.length>0){const s=t[n[0]];if(s!==void 0){this.morphTargetInfluences=[],this.morphTargetDictionary={};for(let r=0,a=s.length;r<a;r++){const o=s[r].name||String(r);this.morphTargetInfluences.push(0),this.morphTargetDictionary[o]=r}}}}}function Al(i,e,t,n,s,r,a){const o=io.distanceSqToPoint(i);if(o<t){const l=new O;io.closestPointToPoint(i,l),l.applyMatrix4(n);const c=s.ray.origin.distanceTo(l);if(c<s.near||c>s.far)return;r.push({distance:c,distanceToRay:Math.sqrt(o),point:l,index:e,face:null,faceIndex:null,barycoord:null,object:a})}}class Ff extends Bt{constructor(e,t,n,s,r,a,o,l,c){super(e,t,n,s,r,a,o,l,c),this.isCanvasTexture=!0,this.needsUpdate=!0}}class Vc extends Bt{constructor(e,t,n=ui,s,r,a,o=fn,l=fn,c,u=ls,d=1){if(u!==ls&&u!==cs)throw new Error("DepthTexture format must be either THREE.DepthFormat or THREE.DepthStencilFormat");const h={width:e,height:t,depth:d};super(h,s,r,a,o,l,u,n,c),this.isDepthTexture=!0,this.flipY=!1,this.generateMipmaps=!1,this.compareFunction=null}copy(e){return super.copy(e),this.source=new wo(Object.assign({},e.image)),this.compareFunction=e.compareFunction,this}toJSON(e){const t=super.toJSON(e);return this.compareFunction!==null&&(t.compareFunction=this.compareFunction),t}}class Co extends jt{constructor(e=[],t=[],n=1,s=0){super(),this.type="PolyhedronGeometry",this.parameters={vertices:e,indices:t,radius:n,detail:s};const r=[],a=[];o(s),c(n),u(),this.setAttribute("position",new Nt(r,3)),this.setAttribute("normal",new Nt(r.slice(),3)),this.setAttribute("uv",new Nt(a,2)),s===0?this.computeVertexNormals():this.normalizeNormals();function o(E){const b=new O,v=new O,T=new O;for(let A=0;A<t.length;A+=3)p(t[A+0],b),p(t[A+1],v),p(t[A+2],T),l(b,v,T,E)}function l(E,b,v,T){const A=T+1,C=[];for(let I=0;I<=A;I++){C[I]=[];const y=E.clone().lerp(v,I/A),S=b.clone().lerp(v,I/A),P=A-I;for(let $=0;$<=P;$++)$===0&&I===A?C[I][$]=y:C[I][$]=y.clone().lerp(S,$/P)}for(let I=0;I<A;I++)for(let y=0;y<2*(A-I)-1;y++){const S=Math.floor(y/2);y%2===0?(h(C[I][S+1]),h(C[I+1][S]),h(C[I][S])):(h(C[I][S+1]),h(C[I+1][S+1]),h(C[I+1][S]))}}function c(E){const b=new O;for(let v=0;v<r.length;v+=3)b.x=r[v+0],b.y=r[v+1],b.z=r[v+2],b.normalize().multiplyScalar(E),r[v+0]=b.x,r[v+1]=b.y,r[v+2]=b.z}function u(){const E=new O;for(let b=0;b<r.length;b+=3){E.x=r[b+0],E.y=r[b+1],E.z=r[b+2];const v=m(E)/2/Math.PI+.5,T=f(E)/Math.PI+.5;a.push(v,1-T)}g(),d()}function d(){for(let E=0;E<a.length;E+=6){const b=a[E+0],v=a[E+2],T=a[E+4],A=Math.max(b,v,T),C=Math.min(b,v,T);A>.9&&C<.1&&(b<.2&&(a[E+0]+=1),v<.2&&(a[E+2]+=1),T<.2&&(a[E+4]+=1))}}function h(E){r.push(E.x,E.y,E.z)}function p(E,b){const v=E*3;b.x=e[v+0],b.y=e[v+1],b.z=e[v+2]}function g(){const E=new O,b=new O,v=new O,T=new O,A=new et,C=new et,I=new et;for(let y=0,S=0;y<r.length;y+=9,S+=6){E.set(r[y+0],r[y+1],r[y+2]),b.set(r[y+3],r[y+4],r[y+5]),v.set(r[y+6],r[y+7],r[y+8]),A.set(a[S+0],a[S+1]),C.set(a[S+2],a[S+3]),I.set(a[S+4],a[S+5]),T.copy(E).add(b).add(v).divideScalar(3);const P=m(T);_(A,S+0,E,P),_(C,S+2,b,P),_(I,S+4,v,P)}}function _(E,b,v,T){T<0&&E.x===1&&(a[b]=E.x-1),v.x===0&&v.z===0&&(a[b]=T/2/Math.PI+.5)}function m(E){return Math.atan2(E.z,-E.x)}function f(E){return Math.atan2(-E.y,Math.sqrt(E.x*E.x+E.z*E.z))}}copy(e){return super.copy(e),this.parameters=Object.assign({},e.parameters),this}static fromJSON(e){return new Co(e.vertices,e.indices,e.radius,e.details)}}class Po extends Co{constructor(e=1,t=0){const n=(1+Math.sqrt(5))/2,s=[-1,n,0,1,n,0,-1,-n,0,1,-n,0,0,-1,n,0,1,n,0,-1,-n,0,1,-n,n,0,-1,n,0,1,-n,0,-1,-n,0,1],r=[0,11,5,0,5,1,0,1,7,0,7,10,0,10,11,1,5,9,5,11,4,11,10,2,10,7,6,7,1,8,3,9,4,3,4,2,3,2,6,3,6,8,3,8,9,4,9,5,2,4,11,6,2,10,8,6,7,9,8,1];super(s,r,e,t),this.type="IcosahedronGeometry",this.parameters={radius:e,detail:t}}static fromJSON(e){return new Po(e.radius,e.detail)}}class Yn extends jt{constructor(e=1,t=1,n=1,s=1){super(),this.type="PlaneGeometry",this.parameters={width:e,height:t,widthSegments:n,heightSegments:s};const r=e/2,a=t/2,o=Math.floor(n),l=Math.floor(s),c=o+1,u=l+1,d=e/o,h=t/l,p=[],g=[],_=[],m=[];for(let f=0;f<u;f++){const E=f*h-a;for(let b=0;b<c;b++){const v=b*d-r;g.push(v,-E,0),_.push(0,0,1),m.push(b/o),m.push(1-f/l)}}for(let f=0;f<l;f++)for(let E=0;E<o;E++){const b=E+c*f,v=E+c*(f+1),T=E+1+c*(f+1),A=E+1+c*f;p.push(b,v,A),p.push(v,T,A)}this.setIndex(p),this.setAttribute("position",new Nt(g,3)),this.setAttribute("normal",new Nt(_,3)),this.setAttribute("uv",new Nt(m,2))}copy(e){return super.copy(e),this.parameters=Object.assign({},e.parameters),this}static fromJSON(e){return new Yn(e.width,e.height,e.widthSegments,e.heightSegments)}}class Mr extends jt{constructor(e=.5,t=1,n=32,s=1,r=0,a=Math.PI*2){super(),this.type="RingGeometry",this.parameters={innerRadius:e,outerRadius:t,thetaSegments:n,phiSegments:s,thetaStart:r,thetaLength:a},n=Math.max(3,n),s=Math.max(1,s);const o=[],l=[],c=[],u=[];let d=e;const h=(t-e)/s,p=new O,g=new et;for(let _=0;_<=s;_++){for(let m=0;m<=n;m++){const f=r+m/n*a;p.x=d*Math.cos(f),p.y=d*Math.sin(f),l.push(p.x,p.y,p.z),c.push(0,0,1),g.x=(p.x/t+1)/2,g.y=(p.y/t+1)/2,u.push(g.x,g.y)}d+=h}for(let _=0;_<s;_++){const m=_*(n+1);for(let f=0;f<n;f++){const E=f+m,b=E,v=E+n+1,T=E+n+2,A=E+1;o.push(b,v,A),o.push(v,T,A)}}this.setIndex(o),this.setAttribute("position",new Nt(l,3)),this.setAttribute("normal",new Nt(c,3)),this.setAttribute("uv",new Nt(u,2))}copy(e){return super.copy(e),this.parameters=Object.assign({},e.parameters),this}static fromJSON(e){return new Mr(e.innerRadius,e.outerRadius,e.thetaSegments,e.phiSegments,e.thetaStart,e.thetaLength)}}class Lo extends jt{constructor(e=1,t=32,n=16,s=0,r=Math.PI*2,a=0,o=Math.PI){super(),this.type="SphereGeometry",this.parameters={radius:e,widthSegments:t,heightSegments:n,phiStart:s,phiLength:r,thetaStart:a,thetaLength:o},t=Math.max(3,Math.floor(t)),n=Math.max(2,Math.floor(n));const l=Math.min(a+o,Math.PI);let c=0;const u=[],d=new O,h=new O,p=[],g=[],_=[],m=[];for(let f=0;f<=n;f++){const E=[],b=f/n;let v=0;f===0&&a===0?v=.5/t:f===n&&l===Math.PI&&(v=-.5/t);for(let T=0;T<=t;T++){const A=T/t;d.x=-e*Math.cos(s+A*r)*Math.sin(a+b*o),d.y=e*Math.cos(a+b*o),d.z=e*Math.sin(s+A*r)*Math.sin(a+b*o),g.push(d.x,d.y,d.z),h.copy(d).normalize(),_.push(h.x,h.y,h.z),m.push(A+v,1-b),E.push(c++)}u.push(E)}for(let f=0;f<n;f++)for(let E=0;E<t;E++){const b=u[f][E+1],v=u[f][E],T=u[f+1][E],A=u[f+1][E+1];(f!==0||a>0)&&p.push(b,v,A),(f!==n-1||l<Math.PI)&&p.push(v,T,A)}this.setIndex(p),this.setAttribute("position",new Nt(g,3)),this.setAttribute("normal",new Nt(_,3)),this.setAttribute("uv",new Nt(m,2))}copy(e){return super.copy(e),this.parameters=Object.assign({},e.parameters),this}static fromJSON(e){return new Lo(e.radius,e.widthSegments,e.heightSegments,e.phiStart,e.phiLength,e.thetaStart,e.thetaLength)}}class Of extends pi{constructor(e){super(),this.isMeshLambertMaterial=!0,this.type="MeshLambertMaterial",this.color=new Le(16777215),this.map=null,this.lightMap=null,this.lightMapIntensity=1,this.aoMap=null,this.aoMapIntensity=1,this.emissive=new Le(0),this.emissiveIntensity=1,this.emissiveMap=null,this.bumpMap=null,this.bumpScale=1,this.normalMap=null,this.normalMapType=Cc,this.normalScale=new et(1,1),this.displacementMap=null,this.displacementScale=1,this.displacementBias=0,this.specularMap=null,this.alphaMap=null,this.envMap=null,this.envMapRotation=new Sn,this.combine=vo,this.reflectivity=1,this.refractionRatio=.98,this.wireframe=!1,this.wireframeLinewidth=1,this.wireframeLinecap="round",this.wireframeLinejoin="round",this.flatShading=!1,this.fog=!0,this.setValues(e)}copy(e){return super.copy(e),this.color.copy(e.color),this.map=e.map,this.lightMap=e.lightMap,this.lightMapIntensity=e.lightMapIntensity,this.aoMap=e.aoMap,this.aoMapIntensity=e.aoMapIntensity,this.emissive.copy(e.emissive),this.emissiveMap=e.emissiveMap,this.emissiveIntensity=e.emissiveIntensity,this.bumpMap=e.bumpMap,this.bumpScale=e.bumpScale,this.normalMap=e.normalMap,this.normalMapType=e.normalMapType,this.normalScale.copy(e.normalScale),this.displacementMap=e.displacementMap,this.displacementScale=e.displacementScale,this.displacementBias=e.displacementBias,this.specularMap=e.specularMap,this.alphaMap=e.alphaMap,this.envMap=e.envMap,this.envMapRotation.copy(e.envMapRotation),this.combine=e.combine,this.reflectivity=e.reflectivity,this.refractionRatio=e.refractionRatio,this.wireframe=e.wireframe,this.wireframeLinewidth=e.wireframeLinewidth,this.wireframeLinecap=e.wireframeLinecap,this.wireframeLinejoin=e.wireframeLinejoin,this.flatShading=e.flatShading,this.fog=e.fog,this}}class Bf extends pi{constructor(e){super(),this.isMeshDepthMaterial=!0,this.type="MeshDepthMaterial",this.depthPacking=Ih,this.map=null,this.alphaMap=null,this.displacementMap=null,this.displacementScale=1,this.displacementBias=0,this.wireframe=!1,this.wireframeLinewidth=1,this.setValues(e)}copy(e){return super.copy(e),this.depthPacking=e.depthPacking,this.map=e.map,this.alphaMap=e.alphaMap,this.displacementMap=e.displacementMap,this.displacementScale=e.displacementScale,this.displacementBias=e.displacementBias,this.wireframe=e.wireframe,this.wireframeLinewidth=e.wireframeLinewidth,this}}class zf extends pi{constructor(e){super(),this.isMeshDistanceMaterial=!0,this.type="MeshDistanceMaterial",this.map=null,this.alphaMap=null,this.displacementMap=null,this.displacementScale=1,this.displacementBias=0,this.setValues(e)}copy(e){return super.copy(e),this.map=e.map,this.alphaMap=e.alphaMap,this.displacementMap=e.displacementMap,this.displacementScale=e.displacementScale,this.displacementBias=e.displacementBias,this}}const Qr={enabled:!1,files:{},add:function(i,e){this.enabled!==!1&&(this.files[i]=e)},get:function(i){if(this.enabled!==!1)return this.files[i]},remove:function(i){delete this.files[i]},clear:function(){this.files={}}};class Vf{constructor(e,t,n){const s=this;let r=!1,a=0,o=0,l;const c=[];this.onStart=void 0,this.onLoad=e,this.onProgress=t,this.onError=n,this.abortController=new AbortController,this.itemStart=function(u){o++,r===!1&&s.onStart!==void 0&&s.onStart(u,a,o),r=!0},this.itemEnd=function(u){a++,s.onProgress!==void 0&&s.onProgress(u,a,o),a===o&&(r=!1,s.onLoad!==void 0&&s.onLoad())},this.itemError=function(u){s.onError!==void 0&&s.onError(u)},this.resolveURL=function(u){return l?l(u):u},this.setURLModifier=function(u){return l=u,this},this.addHandler=function(u,d){return c.push(u,d),this},this.removeHandler=function(u){const d=c.indexOf(u);return d!==-1&&c.splice(d,2),this},this.getHandler=function(u){for(let d=0,h=c.length;d<h;d+=2){const p=c[d],g=c[d+1];if(p.global&&(p.lastIndex=0),p.test(u))return g}return null},this.abort=function(){return this.abortController.abort(),this.abortController=new AbortController,this}}}const Hf=new Vf;class Do{constructor(e){this.manager=e!==void 0?e:Hf,this.crossOrigin="anonymous",this.withCredentials=!1,this.path="",this.resourcePath="",this.requestHeader={}}load(){}loadAsync(e,t){const n=this;return new Promise(function(s,r){n.load(e,s,t,r)})}parse(){}setCrossOrigin(e){return this.crossOrigin=e,this}setWithCredentials(e){return this.withCredentials=e,this}setPath(e){return this.path=e,this}setResourcePath(e){return this.resourcePath=e,this}setRequestHeader(e){return this.requestHeader=e,this}abort(){return this}}Do.DEFAULT_MATERIAL_NAME="__DEFAULT";const Pi=new WeakMap;class Gf extends Do{constructor(e){super(e)}load(e,t,n,s){this.path!==void 0&&(e=this.path+e),e=this.manager.resolveURL(e);const r=this,a=Qr.get(`image:${e}`);if(a!==void 0){if(a.complete===!0)r.manager.itemStart(e),setTimeout(function(){t&&t(a),r.manager.itemEnd(e)},0);else{let d=Pi.get(a);d===void 0&&(d=[],Pi.set(a,d)),d.push({onLoad:t,onError:s})}return a}const o=ds("img");function l(){u(),t&&t(this);const d=Pi.get(this)||[];for(let h=0;h<d.length;h++){const p=d[h];p.onLoad&&p.onLoad(this)}Pi.delete(this),r.manager.itemEnd(e)}function c(d){u(),s&&s(d),Qr.remove(`image:${e}`);const h=Pi.get(this)||[];for(let p=0;p<h.length;p++){const g=h[p];g.onError&&g.onError(d)}Pi.delete(this),r.manager.itemError(e),r.manager.itemEnd(e)}function u(){o.removeEventListener("load",l,!1),o.removeEventListener("error",c,!1)}return o.addEventListener("load",l,!1),o.addEventListener("error",c,!1),e.slice(0,5)!=="data:"&&this.crossOrigin!==void 0&&(o.crossOrigin=this.crossOrigin),Qr.add(`image:${e}`,o),r.manager.itemStart(e),o.src=e,o}}class Wf extends Do{constructor(e){super(e)}load(e,t,n,s){const r=new Bt,a=new Gf(this.manager);return a.setCrossOrigin(this.crossOrigin),a.setPath(this.path),a.load(e,function(o){r.image=o,r.needsUpdate=!0,t!==void 0&&t(r)},n,s),r}}class Hc extends Ut{constructor(e,t=1){super(),this.isLight=!0,this.type="Light",this.color=new Le(e),this.intensity=t}dispose(){}copy(e,t){return super.copy(e,t),this.color.copy(e.color),this.intensity=e.intensity,this}toJSON(e){const t=super.toJSON(e);return t.object.color=this.color.getHex(),t.object.intensity=this.intensity,this.groundColor!==void 0&&(t.object.groundColor=this.groundColor.getHex()),this.distance!==void 0&&(t.object.distance=this.distance),this.angle!==void 0&&(t.object.angle=this.angle),this.decay!==void 0&&(t.object.decay=this.decay),this.penumbra!==void 0&&(t.object.penumbra=this.penumbra),this.shadow!==void 0&&(t.object.shadow=this.shadow.toJSON()),this.target!==void 0&&(t.object.target=this.target.uuid),t}}const ea=new bt,Rl=new O,Cl=new O;class $f{constructor(e){this.camera=e,this.intensity=1,this.bias=0,this.normalBias=0,this.radius=1,this.blurSamples=8,this.mapSize=new et(512,512),this.mapType=Mn,this.map=null,this.mapPass=null,this.matrix=new bt,this.autoUpdate=!0,this.needsUpdate=!1,this._frustum=new Ro,this._frameExtents=new et(1,1),this._viewportCount=1,this._viewports=[new At(0,0,1,1)]}getViewportCount(){return this._viewportCount}getFrustum(){return this._frustum}updateMatrices(e){const t=this.camera,n=this.matrix;Rl.setFromMatrixPosition(e.matrixWorld),t.position.copy(Rl),Cl.setFromMatrixPosition(e.target.matrixWorld),t.lookAt(Cl),t.updateMatrixWorld(),ea.multiplyMatrices(t.projectionMatrix,t.matrixWorldInverse),this._frustum.setFromProjectionMatrix(ea,t.coordinateSystem,t.reversedDepth),t.reversedDepth?n.set(.5,0,0,.5,0,.5,0,.5,0,0,1,0,0,0,0,1):n.set(.5,0,0,.5,0,.5,0,.5,0,0,.5,.5,0,0,0,1),n.multiply(ea)}getViewport(e){return this._viewports[e]}getFrameExtents(){return this._frameExtents}dispose(){this.map&&this.map.dispose(),this.mapPass&&this.mapPass.dispose()}copy(e){return this.camera=e.camera.clone(),this.intensity=e.intensity,this.bias=e.bias,this.radius=e.radius,this.autoUpdate=e.autoUpdate,this.needsUpdate=e.needsUpdate,this.normalBias=e.normalBias,this.blurSamples=e.blurSamples,this.mapSize.copy(e.mapSize),this}clone(){return new this.constructor().copy(this)}toJSON(){const e={};return this.intensity!==1&&(e.intensity=this.intensity),this.bias!==0&&(e.bias=this.bias),this.normalBias!==0&&(e.normalBias=this.normalBias),this.radius!==1&&(e.radius=this.radius),(this.mapSize.x!==512||this.mapSize.y!==512)&&(e.mapSize=this.mapSize.toArray()),e.camera=this.camera.toJSON(!1).object,delete e.camera.matrix,e}}class Gc extends Fc{constructor(e=-1,t=1,n=1,s=-1,r=.1,a=2e3){super(),this.isOrthographicCamera=!0,this.type="OrthographicCamera",this.zoom=1,this.view=null,this.left=e,this.right=t,this.top=n,this.bottom=s,this.near=r,this.far=a,this.updateProjectionMatrix()}copy(e,t){return super.copy(e,t),this.left=e.left,this.right=e.right,this.top=e.top,this.bottom=e.bottom,this.near=e.near,this.far=e.far,this.zoom=e.zoom,this.view=e.view===null?null:Object.assign({},e.view),this}setViewOffset(e,t,n,s,r,a){this.view===null&&(this.view={enabled:!0,fullWidth:1,fullHeight:1,offsetX:0,offsetY:0,width:1,height:1}),this.view.enabled=!0,this.view.fullWidth=e,this.view.fullHeight=t,this.view.offsetX=n,this.view.offsetY=s,this.view.width=r,this.view.height=a,this.updateProjectionMatrix()}clearViewOffset(){this.view!==null&&(this.view.enabled=!1),this.updateProjectionMatrix()}updateProjectionMatrix(){const e=(this.right-this.left)/(2*this.zoom),t=(this.top-this.bottom)/(2*this.zoom),n=(this.right+this.left)/2,s=(this.top+this.bottom)/2;let r=n-e,a=n+e,o=s+t,l=s-t;if(this.view!==null&&this.view.enabled){const c=(this.right-this.left)/this.view.fullWidth/this.zoom,u=(this.top-this.bottom)/this.view.fullHeight/this.zoom;r+=c*this.view.offsetX,a=r+c*this.view.width,o-=u*this.view.offsetY,l=o-u*this.view.height}this.projectionMatrix.makeOrthographic(r,a,o,l,this.near,this.far,this.coordinateSystem,this.reversedDepth),this.projectionMatrixInverse.copy(this.projectionMatrix).invert()}toJSON(e){const t=super.toJSON(e);return t.object.zoom=this.zoom,t.object.left=this.left,t.object.right=this.right,t.object.top=this.top,t.object.bottom=this.bottom,t.object.near=this.near,t.object.far=this.far,this.view!==null&&(t.object.view=Object.assign({},this.view)),t}}class Xf extends $f{constructor(){super(new Gc(-5,5,5,-5,.5,500)),this.isDirectionalLightShadow=!0}}class er extends Hc{constructor(e,t){super(e,t),this.isDirectionalLight=!0,this.type="DirectionalLight",this.position.copy(Ut.DEFAULT_UP),this.updateMatrix(),this.target=new Ut,this.shadow=new Xf}dispose(){this.shadow.dispose()}copy(e){return super.copy(e),this.target=e.target.clone(),this.shadow=e.shadow.clone(),this}}class Io extends Hc{constructor(e,t){super(e,t),this.isAmbientLight=!0,this.type="AmbientLight"}}class qf extends Wt{constructor(e=[]){super(),this.isArrayCamera=!0,this.isMultiViewCamera=!1,this.cameras=e}}const Pl=new bt;class Yf{constructor(e,t,n=0,s=1/0){this.ray=new vr(e,t),this.near=n,this.far=s,this.camera=null,this.layers=new Ao,this.params={Mesh:{},Line:{threshold:1},LOD:{},Points:{threshold:1},Sprite:{}}}set(e,t){this.ray.set(e,t)}setFromCamera(e,t){t.isPerspectiveCamera?(this.ray.origin.setFromMatrixPosition(t.matrixWorld),this.ray.direction.set(e.x,e.y,.5).unproject(t).sub(this.ray.origin).normalize(),this.camera=t):t.isOrthographicCamera?(this.ray.origin.set(e.x,e.y,(t.near+t.far)/(t.near-t.far)).unproject(t),this.ray.direction.set(0,0,-1).transformDirection(t.matrixWorld),this.camera=t):console.error("THREE.Raycaster: Unsupported camera type: "+t.type)}setFromXRController(e){return Pl.identity().extractRotation(e.matrixWorld),this.ray.origin.setFromMatrixPosition(e.matrixWorld),this.ray.direction.set(0,0,-1).applyMatrix4(Pl),this}intersectObject(e,t=!0,n=[]){return so(e,this,n,t),n.sort(Ll),n}intersectObjects(e,t=!0,n=[]){for(let s=0,r=e.length;s<r;s++)so(e[s],this,n,t);return n.sort(Ll),n}}function Ll(i,e){return i.distance-e.distance}function so(i,e,t,n){let s=!0;if(i.layers.test(e.layers)&&i.raycast(e,t)===!1&&(s=!1),s===!0&&n===!0){const r=i.children;for(let a=0,o=r.length;a<o;a++)so(r[a],e,t,!0)}}function Dl(i,e,t,n){const s=jf(n);switch(t){case Ec:return i*e;case wc:return i*e/s.components*s.byteLength;case yo:return i*e/s.components*s.byteLength;case Ac:return i*e*2/s.components*s.byteLength;case bo:return i*e*2/s.components*s.byteLength;case Tc:return i*e*3/s.components*s.byteLength;case hn:return i*e*4/s.components*s.byteLength;case Eo:return i*e*4/s.components*s.byteLength;case js:case Zs:return Math.floor((i+3)/4)*Math.floor((e+3)/4)*8;case Ks:case Js:return Math.floor((i+3)/4)*Math.floor((e+3)/4)*16;case Da:case Ua:return Math.max(i,16)*Math.max(e,8)/4;case La:case Ia:return Math.max(i,8)*Math.max(e,8)/2;case ka:case Na:return Math.floor((i+3)/4)*Math.floor((e+3)/4)*8;case Fa:return Math.floor((i+3)/4)*Math.floor((e+3)/4)*16;case Oa:return Math.floor((i+3)/4)*Math.floor((e+3)/4)*16;case Ba:return Math.floor((i+4)/5)*Math.floor((e+3)/4)*16;case za:return Math.floor((i+4)/5)*Math.floor((e+4)/5)*16;case Va:return Math.floor((i+5)/6)*Math.floor((e+4)/5)*16;case Ha:return Math.floor((i+5)/6)*Math.floor((e+5)/6)*16;case Ga:return Math.floor((i+7)/8)*Math.floor((e+4)/5)*16;case Wa:return Math.floor((i+7)/8)*Math.floor((e+5)/6)*16;case $a:return Math.floor((i+7)/8)*Math.floor((e+7)/8)*16;case Xa:return Math.floor((i+9)/10)*Math.floor((e+4)/5)*16;case qa:return Math.floor((i+9)/10)*Math.floor((e+5)/6)*16;case Ya:return Math.floor((i+9)/10)*Math.floor((e+7)/8)*16;case ja:return Math.floor((i+9)/10)*Math.floor((e+9)/10)*16;case Za:return Math.floor((i+11)/12)*Math.floor((e+9)/10)*16;case Ka:return Math.floor((i+11)/12)*Math.floor((e+11)/12)*16;case Qs:case Ja:case Qa:return Math.ceil(i/4)*Math.ceil(e/4)*16;case Rc:case eo:return Math.ceil(i/4)*Math.ceil(e/4)*8;case to:case no:return Math.ceil(i/4)*Math.ceil(e/4)*16}throw new Error(`Unable to determine texture byte length for ${t} format.`)}function jf(i){switch(i){case Mn:case Sc:return{byteLength:1,components:1};case as:case yc:case ps:return{byteLength:2,components:1};case Mo:case So:return{byteLength:2,components:4};case ui:case xo:case Pn:return{byteLength:4,components:1};case bc:return{byteLength:4,components:3}}throw new Error(`Unknown texture type ${i}.`)}typeof __THREE_DEVTOOLS__<"u"&&__THREE_DEVTOOLS__.dispatchEvent(new CustomEvent("register",{detail:{revision:go}}));typeof window<"u"&&(window.__THREE__?console.warn("WARNING: Multiple instances of Three.js being imported."):window.__THREE__=go);/**
 * @license
 * Copyright 2010-2025 Three.js Authors
 * SPDX-License-Identifier: MIT
 */function Wc(){let i=null,e=!1,t=null,n=null;function s(r,a){t(r,a),n=i.requestAnimationFrame(s)}return{start:function(){e!==!0&&t!==null&&(n=i.requestAnimationFrame(s),e=!0)},stop:function(){i.cancelAnimationFrame(n),e=!1},setAnimationLoop:function(r){t=r},setContext:function(r){i=r}}}function Zf(i){const e=new WeakMap;function t(o,l){const c=o.array,u=o.usage,d=c.byteLength,h=i.createBuffer();i.bindBuffer(l,h),i.bufferData(l,c,u),o.onUploadCallback();let p;if(c instanceof Float32Array)p=i.FLOAT;else if(typeof Float16Array<"u"&&c instanceof Float16Array)p=i.HALF_FLOAT;else if(c instanceof Uint16Array)o.isFloat16BufferAttribute?p=i.HALF_FLOAT:p=i.UNSIGNED_SHORT;else if(c instanceof Int16Array)p=i.SHORT;else if(c instanceof Uint32Array)p=i.UNSIGNED_INT;else if(c instanceof Int32Array)p=i.INT;else if(c instanceof Int8Array)p=i.BYTE;else if(c instanceof Uint8Array)p=i.UNSIGNED_BYTE;else if(c instanceof Uint8ClampedArray)p=i.UNSIGNED_BYTE;else throw new Error("THREE.WebGLAttributes: Unsupported buffer data format: "+c);return{buffer:h,type:p,bytesPerElement:c.BYTES_PER_ELEMENT,version:o.version,size:d}}function n(o,l,c){const u=l.array,d=l.updateRanges;if(i.bindBuffer(c,o),d.length===0)i.bufferSubData(c,0,u);else{d.sort((p,g)=>p.start-g.start);let h=0;for(let p=1;p<d.length;p++){const g=d[h],_=d[p];_.start<=g.start+g.count+1?g.count=Math.max(g.count,_.start+_.count-g.start):(++h,d[h]=_)}d.length=h+1;for(let p=0,g=d.length;p<g;p++){const _=d[p];i.bufferSubData(c,_.start*u.BYTES_PER_ELEMENT,u,_.start,_.count)}l.clearUpdateRanges()}l.onUploadCallback()}function s(o){return o.isInterleavedBufferAttribute&&(o=o.data),e.get(o)}function r(o){o.isInterleavedBufferAttribute&&(o=o.data);const l=e.get(o);l&&(i.deleteBuffer(l.buffer),e.delete(o))}function a(o,l){if(o.isInterleavedBufferAttribute&&(o=o.data),o.isGLBufferAttribute){const u=e.get(o);(!u||u.version<o.version)&&e.set(o,{buffer:o.buffer,type:o.type,bytesPerElement:o.elementSize,version:o.version});return}const c=e.get(o);if(c===void 0)e.set(o,t(o,l));else if(c.version<o.version){if(c.size!==o.array.byteLength)throw new Error("THREE.WebGLAttributes: The size of the buffer attribute's array buffer does not match the original size. Resizing buffer attributes is not supported.");n(c.buffer,o,l),c.version=o.version}}return{get:s,remove:r,update:a}}var Kf=`#ifdef USE_ALPHAHASH
	if ( diffuseColor.a < getAlphaHashThreshold( vPosition ) ) discard;
#endif`,Jf=`#ifdef USE_ALPHAHASH
	const float ALPHA_HASH_SCALE = 0.05;
	float hash2D( vec2 value ) {
		return fract( 1.0e4 * sin( 17.0 * value.x + 0.1 * value.y ) * ( 0.1 + abs( sin( 13.0 * value.y + value.x ) ) ) );
	}
	float hash3D( vec3 value ) {
		return hash2D( vec2( hash2D( value.xy ), value.z ) );
	}
	float getAlphaHashThreshold( vec3 position ) {
		float maxDeriv = max(
			length( dFdx( position.xyz ) ),
			length( dFdy( position.xyz ) )
		);
		float pixScale = 1.0 / ( ALPHA_HASH_SCALE * maxDeriv );
		vec2 pixScales = vec2(
			exp2( floor( log2( pixScale ) ) ),
			exp2( ceil( log2( pixScale ) ) )
		);
		vec2 alpha = vec2(
			hash3D( floor( pixScales.x * position.xyz ) ),
			hash3D( floor( pixScales.y * position.xyz ) )
		);
		float lerpFactor = fract( log2( pixScale ) );
		float x = ( 1.0 - lerpFactor ) * alpha.x + lerpFactor * alpha.y;
		float a = min( lerpFactor, 1.0 - lerpFactor );
		vec3 cases = vec3(
			x * x / ( 2.0 * a * ( 1.0 - a ) ),
			( x - 0.5 * a ) / ( 1.0 - a ),
			1.0 - ( ( 1.0 - x ) * ( 1.0 - x ) / ( 2.0 * a * ( 1.0 - a ) ) )
		);
		float threshold = ( x < ( 1.0 - a ) )
			? ( ( x < a ) ? cases.x : cases.y )
			: cases.z;
		return clamp( threshold , 1.0e-6, 1.0 );
	}
#endif`,Qf=`#ifdef USE_ALPHAMAP
	diffuseColor.a *= texture2D( alphaMap, vAlphaMapUv ).g;
#endif`,ep=`#ifdef USE_ALPHAMAP
	uniform sampler2D alphaMap;
#endif`,tp=`#ifdef USE_ALPHATEST
	#ifdef ALPHA_TO_COVERAGE
	diffuseColor.a = smoothstep( alphaTest, alphaTest + fwidth( diffuseColor.a ), diffuseColor.a );
	if ( diffuseColor.a == 0.0 ) discard;
	#else
	if ( diffuseColor.a < alphaTest ) discard;
	#endif
#endif`,np=`#ifdef USE_ALPHATEST
	uniform float alphaTest;
#endif`,ip=`#ifdef USE_AOMAP
	float ambientOcclusion = ( texture2D( aoMap, vAoMapUv ).r - 1.0 ) * aoMapIntensity + 1.0;
	reflectedLight.indirectDiffuse *= ambientOcclusion;
	#if defined( USE_CLEARCOAT ) 
		clearcoatSpecularIndirect *= ambientOcclusion;
	#endif
	#if defined( USE_SHEEN ) 
		sheenSpecularIndirect *= ambientOcclusion;
	#endif
	#if defined( USE_ENVMAP ) && defined( STANDARD )
		float dotNV = saturate( dot( geometryNormal, geometryViewDir ) );
		reflectedLight.indirectSpecular *= computeSpecularOcclusion( dotNV, ambientOcclusion, material.roughness );
	#endif
#endif`,sp=`#ifdef USE_AOMAP
	uniform sampler2D aoMap;
	uniform float aoMapIntensity;
#endif`,rp=`#ifdef USE_BATCHING
	#if ! defined( GL_ANGLE_multi_draw )
	#define gl_DrawID _gl_DrawID
	uniform int _gl_DrawID;
	#endif
	uniform highp sampler2D batchingTexture;
	uniform highp usampler2D batchingIdTexture;
	mat4 getBatchingMatrix( const in float i ) {
		int size = textureSize( batchingTexture, 0 ).x;
		int j = int( i ) * 4;
		int x = j % size;
		int y = j / size;
		vec4 v1 = texelFetch( batchingTexture, ivec2( x, y ), 0 );
		vec4 v2 = texelFetch( batchingTexture, ivec2( x + 1, y ), 0 );
		vec4 v3 = texelFetch( batchingTexture, ivec2( x + 2, y ), 0 );
		vec4 v4 = texelFetch( batchingTexture, ivec2( x + 3, y ), 0 );
		return mat4( v1, v2, v3, v4 );
	}
	float getIndirectIndex( const in int i ) {
		int size = textureSize( batchingIdTexture, 0 ).x;
		int x = i % size;
		int y = i / size;
		return float( texelFetch( batchingIdTexture, ivec2( x, y ), 0 ).r );
	}
#endif
#ifdef USE_BATCHING_COLOR
	uniform sampler2D batchingColorTexture;
	vec3 getBatchingColor( const in float i ) {
		int size = textureSize( batchingColorTexture, 0 ).x;
		int j = int( i );
		int x = j % size;
		int y = j / size;
		return texelFetch( batchingColorTexture, ivec2( x, y ), 0 ).rgb;
	}
#endif`,ap=`#ifdef USE_BATCHING
	mat4 batchingMatrix = getBatchingMatrix( getIndirectIndex( gl_DrawID ) );
#endif`,op=`vec3 transformed = vec3( position );
#ifdef USE_ALPHAHASH
	vPosition = vec3( position );
#endif`,lp=`vec3 objectNormal = vec3( normal );
#ifdef USE_TANGENT
	vec3 objectTangent = vec3( tangent.xyz );
#endif`,cp=`float G_BlinnPhong_Implicit( ) {
	return 0.25;
}
float D_BlinnPhong( const in float shininess, const in float dotNH ) {
	return RECIPROCAL_PI * ( shininess * 0.5 + 1.0 ) * pow( dotNH, shininess );
}
vec3 BRDF_BlinnPhong( const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal, const in vec3 specularColor, const in float shininess ) {
	vec3 halfDir = normalize( lightDir + viewDir );
	float dotNH = saturate( dot( normal, halfDir ) );
	float dotVH = saturate( dot( viewDir, halfDir ) );
	vec3 F = F_Schlick( specularColor, 1.0, dotVH );
	float G = G_BlinnPhong_Implicit( );
	float D = D_BlinnPhong( shininess, dotNH );
	return F * ( G * D );
} // validated`,up=`#ifdef USE_IRIDESCENCE
	const mat3 XYZ_TO_REC709 = mat3(
		 3.2404542, -0.9692660,  0.0556434,
		-1.5371385,  1.8760108, -0.2040259,
		-0.4985314,  0.0415560,  1.0572252
	);
	vec3 Fresnel0ToIor( vec3 fresnel0 ) {
		vec3 sqrtF0 = sqrt( fresnel0 );
		return ( vec3( 1.0 ) + sqrtF0 ) / ( vec3( 1.0 ) - sqrtF0 );
	}
	vec3 IorToFresnel0( vec3 transmittedIor, float incidentIor ) {
		return pow2( ( transmittedIor - vec3( incidentIor ) ) / ( transmittedIor + vec3( incidentIor ) ) );
	}
	float IorToFresnel0( float transmittedIor, float incidentIor ) {
		return pow2( ( transmittedIor - incidentIor ) / ( transmittedIor + incidentIor ));
	}
	vec3 evalSensitivity( float OPD, vec3 shift ) {
		float phase = 2.0 * PI * OPD * 1.0e-9;
		vec3 val = vec3( 5.4856e-13, 4.4201e-13, 5.2481e-13 );
		vec3 pos = vec3( 1.6810e+06, 1.7953e+06, 2.2084e+06 );
		vec3 var = vec3( 4.3278e+09, 9.3046e+09, 6.6121e+09 );
		vec3 xyz = val * sqrt( 2.0 * PI * var ) * cos( pos * phase + shift ) * exp( - pow2( phase ) * var );
		xyz.x += 9.7470e-14 * sqrt( 2.0 * PI * 4.5282e+09 ) * cos( 2.2399e+06 * phase + shift[ 0 ] ) * exp( - 4.5282e+09 * pow2( phase ) );
		xyz /= 1.0685e-7;
		vec3 rgb = XYZ_TO_REC709 * xyz;
		return rgb;
	}
	vec3 evalIridescence( float outsideIOR, float eta2, float cosTheta1, float thinFilmThickness, vec3 baseF0 ) {
		vec3 I;
		float iridescenceIOR = mix( outsideIOR, eta2, smoothstep( 0.0, 0.03, thinFilmThickness ) );
		float sinTheta2Sq = pow2( outsideIOR / iridescenceIOR ) * ( 1.0 - pow2( cosTheta1 ) );
		float cosTheta2Sq = 1.0 - sinTheta2Sq;
		if ( cosTheta2Sq < 0.0 ) {
			return vec3( 1.0 );
		}
		float cosTheta2 = sqrt( cosTheta2Sq );
		float R0 = IorToFresnel0( iridescenceIOR, outsideIOR );
		float R12 = F_Schlick( R0, 1.0, cosTheta1 );
		float T121 = 1.0 - R12;
		float phi12 = 0.0;
		if ( iridescenceIOR < outsideIOR ) phi12 = PI;
		float phi21 = PI - phi12;
		vec3 baseIOR = Fresnel0ToIor( clamp( baseF0, 0.0, 0.9999 ) );		vec3 R1 = IorToFresnel0( baseIOR, iridescenceIOR );
		vec3 R23 = F_Schlick( R1, 1.0, cosTheta2 );
		vec3 phi23 = vec3( 0.0 );
		if ( baseIOR[ 0 ] < iridescenceIOR ) phi23[ 0 ] = PI;
		if ( baseIOR[ 1 ] < iridescenceIOR ) phi23[ 1 ] = PI;
		if ( baseIOR[ 2 ] < iridescenceIOR ) phi23[ 2 ] = PI;
		float OPD = 2.0 * iridescenceIOR * thinFilmThickness * cosTheta2;
		vec3 phi = vec3( phi21 ) + phi23;
		vec3 R123 = clamp( R12 * R23, 1e-5, 0.9999 );
		vec3 r123 = sqrt( R123 );
		vec3 Rs = pow2( T121 ) * R23 / ( vec3( 1.0 ) - R123 );
		vec3 C0 = R12 + Rs;
		I = C0;
		vec3 Cm = Rs - T121;
		for ( int m = 1; m <= 2; ++ m ) {
			Cm *= r123;
			vec3 Sm = 2.0 * evalSensitivity( float( m ) * OPD, float( m ) * phi );
			I += Cm * Sm;
		}
		return max( I, vec3( 0.0 ) );
	}
#endif`,dp=`#ifdef USE_BUMPMAP
	uniform sampler2D bumpMap;
	uniform float bumpScale;
	vec2 dHdxy_fwd() {
		vec2 dSTdx = dFdx( vBumpMapUv );
		vec2 dSTdy = dFdy( vBumpMapUv );
		float Hll = bumpScale * texture2D( bumpMap, vBumpMapUv ).x;
		float dBx = bumpScale * texture2D( bumpMap, vBumpMapUv + dSTdx ).x - Hll;
		float dBy = bumpScale * texture2D( bumpMap, vBumpMapUv + dSTdy ).x - Hll;
		return vec2( dBx, dBy );
	}
	vec3 perturbNormalArb( vec3 surf_pos, vec3 surf_norm, vec2 dHdxy, float faceDirection ) {
		vec3 vSigmaX = normalize( dFdx( surf_pos.xyz ) );
		vec3 vSigmaY = normalize( dFdy( surf_pos.xyz ) );
		vec3 vN = surf_norm;
		vec3 R1 = cross( vSigmaY, vN );
		vec3 R2 = cross( vN, vSigmaX );
		float fDet = dot( vSigmaX, R1 ) * faceDirection;
		vec3 vGrad = sign( fDet ) * ( dHdxy.x * R1 + dHdxy.y * R2 );
		return normalize( abs( fDet ) * surf_norm - vGrad );
	}
#endif`,hp=`#if NUM_CLIPPING_PLANES > 0
	vec4 plane;
	#ifdef ALPHA_TO_COVERAGE
		float distanceToPlane, distanceGradient;
		float clipOpacity = 1.0;
		#pragma unroll_loop_start
		for ( int i = 0; i < UNION_CLIPPING_PLANES; i ++ ) {
			plane = clippingPlanes[ i ];
			distanceToPlane = - dot( vClipPosition, plane.xyz ) + plane.w;
			distanceGradient = fwidth( distanceToPlane ) / 2.0;
			clipOpacity *= smoothstep( - distanceGradient, distanceGradient, distanceToPlane );
			if ( clipOpacity == 0.0 ) discard;
		}
		#pragma unroll_loop_end
		#if UNION_CLIPPING_PLANES < NUM_CLIPPING_PLANES
			float unionClipOpacity = 1.0;
			#pragma unroll_loop_start
			for ( int i = UNION_CLIPPING_PLANES; i < NUM_CLIPPING_PLANES; i ++ ) {
				plane = clippingPlanes[ i ];
				distanceToPlane = - dot( vClipPosition, plane.xyz ) + plane.w;
				distanceGradient = fwidth( distanceToPlane ) / 2.0;
				unionClipOpacity *= 1.0 - smoothstep( - distanceGradient, distanceGradient, distanceToPlane );
			}
			#pragma unroll_loop_end
			clipOpacity *= 1.0 - unionClipOpacity;
		#endif
		diffuseColor.a *= clipOpacity;
		if ( diffuseColor.a == 0.0 ) discard;
	#else
		#pragma unroll_loop_start
		for ( int i = 0; i < UNION_CLIPPING_PLANES; i ++ ) {
			plane = clippingPlanes[ i ];
			if ( dot( vClipPosition, plane.xyz ) > plane.w ) discard;
		}
		#pragma unroll_loop_end
		#if UNION_CLIPPING_PLANES < NUM_CLIPPING_PLANES
			bool clipped = true;
			#pragma unroll_loop_start
			for ( int i = UNION_CLIPPING_PLANES; i < NUM_CLIPPING_PLANES; i ++ ) {
				plane = clippingPlanes[ i ];
				clipped = ( dot( vClipPosition, plane.xyz ) > plane.w ) && clipped;
			}
			#pragma unroll_loop_end
			if ( clipped ) discard;
		#endif
	#endif
#endif`,fp=`#if NUM_CLIPPING_PLANES > 0
	varying vec3 vClipPosition;
	uniform vec4 clippingPlanes[ NUM_CLIPPING_PLANES ];
#endif`,pp=`#if NUM_CLIPPING_PLANES > 0
	varying vec3 vClipPosition;
#endif`,mp=`#if NUM_CLIPPING_PLANES > 0
	vClipPosition = - mvPosition.xyz;
#endif`,gp=`#if defined( USE_COLOR_ALPHA )
	diffuseColor *= vColor;
#elif defined( USE_COLOR )
	diffuseColor.rgb *= vColor;
#endif`,_p=`#if defined( USE_COLOR_ALPHA )
	varying vec4 vColor;
#elif defined( USE_COLOR )
	varying vec3 vColor;
#endif`,vp=`#if defined( USE_COLOR_ALPHA )
	varying vec4 vColor;
#elif defined( USE_COLOR ) || defined( USE_INSTANCING_COLOR ) || defined( USE_BATCHING_COLOR )
	varying vec3 vColor;
#endif`,xp=`#if defined( USE_COLOR_ALPHA )
	vColor = vec4( 1.0 );
#elif defined( USE_COLOR ) || defined( USE_INSTANCING_COLOR ) || defined( USE_BATCHING_COLOR )
	vColor = vec3( 1.0 );
#endif
#ifdef USE_COLOR
	vColor *= color;
#endif
#ifdef USE_INSTANCING_COLOR
	vColor.xyz *= instanceColor.xyz;
#endif
#ifdef USE_BATCHING_COLOR
	vec3 batchingColor = getBatchingColor( getIndirectIndex( gl_DrawID ) );
	vColor.xyz *= batchingColor.xyz;
#endif`,Mp=`#define PI 3.141592653589793
#define PI2 6.283185307179586
#define PI_HALF 1.5707963267948966
#define RECIPROCAL_PI 0.3183098861837907
#define RECIPROCAL_PI2 0.15915494309189535
#define EPSILON 1e-6
#ifndef saturate
#define saturate( a ) clamp( a, 0.0, 1.0 )
#endif
#define whiteComplement( a ) ( 1.0 - saturate( a ) )
float pow2( const in float x ) { return x*x; }
vec3 pow2( const in vec3 x ) { return x*x; }
float pow3( const in float x ) { return x*x*x; }
float pow4( const in float x ) { float x2 = x*x; return x2*x2; }
float max3( const in vec3 v ) { return max( max( v.x, v.y ), v.z ); }
float average( const in vec3 v ) { return dot( v, vec3( 0.3333333 ) ); }
highp float rand( const in vec2 uv ) {
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract( sin( sn ) * c );
}
#ifdef HIGH_PRECISION
	float precisionSafeLength( vec3 v ) { return length( v ); }
#else
	float precisionSafeLength( vec3 v ) {
		float maxComponent = max3( abs( v ) );
		return length( v / maxComponent ) * maxComponent;
	}
#endif
struct IncidentLight {
	vec3 color;
	vec3 direction;
	bool visible;
};
struct ReflectedLight {
	vec3 directDiffuse;
	vec3 directSpecular;
	vec3 indirectDiffuse;
	vec3 indirectSpecular;
};
#ifdef USE_ALPHAHASH
	varying vec3 vPosition;
#endif
vec3 transformDirection( in vec3 dir, in mat4 matrix ) {
	return normalize( ( matrix * vec4( dir, 0.0 ) ).xyz );
}
vec3 inverseTransformDirection( in vec3 dir, in mat4 matrix ) {
	return normalize( ( vec4( dir, 0.0 ) * matrix ).xyz );
}
mat3 transposeMat3( const in mat3 m ) {
	mat3 tmp;
	tmp[ 0 ] = vec3( m[ 0 ].x, m[ 1 ].x, m[ 2 ].x );
	tmp[ 1 ] = vec3( m[ 0 ].y, m[ 1 ].y, m[ 2 ].y );
	tmp[ 2 ] = vec3( m[ 0 ].z, m[ 1 ].z, m[ 2 ].z );
	return tmp;
}
bool isPerspectiveMatrix( mat4 m ) {
	return m[ 2 ][ 3 ] == - 1.0;
}
vec2 equirectUv( in vec3 dir ) {
	float u = atan( dir.z, dir.x ) * RECIPROCAL_PI2 + 0.5;
	float v = asin( clamp( dir.y, - 1.0, 1.0 ) ) * RECIPROCAL_PI + 0.5;
	return vec2( u, v );
}
vec3 BRDF_Lambert( const in vec3 diffuseColor ) {
	return RECIPROCAL_PI * diffuseColor;
}
vec3 F_Schlick( const in vec3 f0, const in float f90, const in float dotVH ) {
	float fresnel = exp2( ( - 5.55473 * dotVH - 6.98316 ) * dotVH );
	return f0 * ( 1.0 - fresnel ) + ( f90 * fresnel );
}
float F_Schlick( const in float f0, const in float f90, const in float dotVH ) {
	float fresnel = exp2( ( - 5.55473 * dotVH - 6.98316 ) * dotVH );
	return f0 * ( 1.0 - fresnel ) + ( f90 * fresnel );
} // validated`,Sp=`#ifdef ENVMAP_TYPE_CUBE_UV
	#define cubeUV_minMipLevel 4.0
	#define cubeUV_minTileSize 16.0
	float getFace( vec3 direction ) {
		vec3 absDirection = abs( direction );
		float face = - 1.0;
		if ( absDirection.x > absDirection.z ) {
			if ( absDirection.x > absDirection.y )
				face = direction.x > 0.0 ? 0.0 : 3.0;
			else
				face = direction.y > 0.0 ? 1.0 : 4.0;
		} else {
			if ( absDirection.z > absDirection.y )
				face = direction.z > 0.0 ? 2.0 : 5.0;
			else
				face = direction.y > 0.0 ? 1.0 : 4.0;
		}
		return face;
	}
	vec2 getUV( vec3 direction, float face ) {
		vec2 uv;
		if ( face == 0.0 ) {
			uv = vec2( direction.z, direction.y ) / abs( direction.x );
		} else if ( face == 1.0 ) {
			uv = vec2( - direction.x, - direction.z ) / abs( direction.y );
		} else if ( face == 2.0 ) {
			uv = vec2( - direction.x, direction.y ) / abs( direction.z );
		} else if ( face == 3.0 ) {
			uv = vec2( - direction.z, direction.y ) / abs( direction.x );
		} else if ( face == 4.0 ) {
			uv = vec2( - direction.x, direction.z ) / abs( direction.y );
		} else {
			uv = vec2( direction.x, direction.y ) / abs( direction.z );
		}
		return 0.5 * ( uv + 1.0 );
	}
	vec3 bilinearCubeUV( sampler2D envMap, vec3 direction, float mipInt ) {
		float face = getFace( direction );
		float filterInt = max( cubeUV_minMipLevel - mipInt, 0.0 );
		mipInt = max( mipInt, cubeUV_minMipLevel );
		float faceSize = exp2( mipInt );
		highp vec2 uv = getUV( direction, face ) * ( faceSize - 2.0 ) + 1.0;
		if ( face > 2.0 ) {
			uv.y += faceSize;
			face -= 3.0;
		}
		uv.x += face * faceSize;
		uv.x += filterInt * 3.0 * cubeUV_minTileSize;
		uv.y += 4.0 * ( exp2( CUBEUV_MAX_MIP ) - faceSize );
		uv.x *= CUBEUV_TEXEL_WIDTH;
		uv.y *= CUBEUV_TEXEL_HEIGHT;
		#ifdef texture2DGradEXT
			return texture2DGradEXT( envMap, uv, vec2( 0.0 ), vec2( 0.0 ) ).rgb;
		#else
			return texture2D( envMap, uv ).rgb;
		#endif
	}
	#define cubeUV_r0 1.0
	#define cubeUV_m0 - 2.0
	#define cubeUV_r1 0.8
	#define cubeUV_m1 - 1.0
	#define cubeUV_r4 0.4
	#define cubeUV_m4 2.0
	#define cubeUV_r5 0.305
	#define cubeUV_m5 3.0
	#define cubeUV_r6 0.21
	#define cubeUV_m6 4.0
	float roughnessToMip( float roughness ) {
		float mip = 0.0;
		if ( roughness >= cubeUV_r1 ) {
			mip = ( cubeUV_r0 - roughness ) * ( cubeUV_m1 - cubeUV_m0 ) / ( cubeUV_r0 - cubeUV_r1 ) + cubeUV_m0;
		} else if ( roughness >= cubeUV_r4 ) {
			mip = ( cubeUV_r1 - roughness ) * ( cubeUV_m4 - cubeUV_m1 ) / ( cubeUV_r1 - cubeUV_r4 ) + cubeUV_m1;
		} else if ( roughness >= cubeUV_r5 ) {
			mip = ( cubeUV_r4 - roughness ) * ( cubeUV_m5 - cubeUV_m4 ) / ( cubeUV_r4 - cubeUV_r5 ) + cubeUV_m4;
		} else if ( roughness >= cubeUV_r6 ) {
			mip = ( cubeUV_r5 - roughness ) * ( cubeUV_m6 - cubeUV_m5 ) / ( cubeUV_r5 - cubeUV_r6 ) + cubeUV_m5;
		} else {
			mip = - 2.0 * log2( 1.16 * roughness );		}
		return mip;
	}
	vec4 textureCubeUV( sampler2D envMap, vec3 sampleDir, float roughness ) {
		float mip = clamp( roughnessToMip( roughness ), cubeUV_m0, CUBEUV_MAX_MIP );
		float mipF = fract( mip );
		float mipInt = floor( mip );
		vec3 color0 = bilinearCubeUV( envMap, sampleDir, mipInt );
		if ( mipF == 0.0 ) {
			return vec4( color0, 1.0 );
		} else {
			vec3 color1 = bilinearCubeUV( envMap, sampleDir, mipInt + 1.0 );
			return vec4( mix( color0, color1, mipF ), 1.0 );
		}
	}
#endif`,yp=`vec3 transformedNormal = objectNormal;
#ifdef USE_TANGENT
	vec3 transformedTangent = objectTangent;
#endif
#ifdef USE_BATCHING
	mat3 bm = mat3( batchingMatrix );
	transformedNormal /= vec3( dot( bm[ 0 ], bm[ 0 ] ), dot( bm[ 1 ], bm[ 1 ] ), dot( bm[ 2 ], bm[ 2 ] ) );
	transformedNormal = bm * transformedNormal;
	#ifdef USE_TANGENT
		transformedTangent = bm * transformedTangent;
	#endif
#endif
#ifdef USE_INSTANCING
	mat3 im = mat3( instanceMatrix );
	transformedNormal /= vec3( dot( im[ 0 ], im[ 0 ] ), dot( im[ 1 ], im[ 1 ] ), dot( im[ 2 ], im[ 2 ] ) );
	transformedNormal = im * transformedNormal;
	#ifdef USE_TANGENT
		transformedTangent = im * transformedTangent;
	#endif
#endif
transformedNormal = normalMatrix * transformedNormal;
#ifdef FLIP_SIDED
	transformedNormal = - transformedNormal;
#endif
#ifdef USE_TANGENT
	transformedTangent = ( modelViewMatrix * vec4( transformedTangent, 0.0 ) ).xyz;
	#ifdef FLIP_SIDED
		transformedTangent = - transformedTangent;
	#endif
#endif`,bp=`#ifdef USE_DISPLACEMENTMAP
	uniform sampler2D displacementMap;
	uniform float displacementScale;
	uniform float displacementBias;
#endif`,Ep=`#ifdef USE_DISPLACEMENTMAP
	transformed += normalize( objectNormal ) * ( texture2D( displacementMap, vDisplacementMapUv ).x * displacementScale + displacementBias );
#endif`,Tp=`#ifdef USE_EMISSIVEMAP
	vec4 emissiveColor = texture2D( emissiveMap, vEmissiveMapUv );
	#ifdef DECODE_VIDEO_TEXTURE_EMISSIVE
		emissiveColor = sRGBTransferEOTF( emissiveColor );
	#endif
	totalEmissiveRadiance *= emissiveColor.rgb;
#endif`,wp=`#ifdef USE_EMISSIVEMAP
	uniform sampler2D emissiveMap;
#endif`,Ap="gl_FragColor = linearToOutputTexel( gl_FragColor );",Rp=`vec4 LinearTransferOETF( in vec4 value ) {
	return value;
}
vec4 sRGBTransferEOTF( in vec4 value ) {
	return vec4( mix( pow( value.rgb * 0.9478672986 + vec3( 0.0521327014 ), vec3( 2.4 ) ), value.rgb * 0.0773993808, vec3( lessThanEqual( value.rgb, vec3( 0.04045 ) ) ) ), value.a );
}
vec4 sRGBTransferOETF( in vec4 value ) {
	return vec4( mix( pow( value.rgb, vec3( 0.41666 ) ) * 1.055 - vec3( 0.055 ), value.rgb * 12.92, vec3( lessThanEqual( value.rgb, vec3( 0.0031308 ) ) ) ), value.a );
}`,Cp=`#ifdef USE_ENVMAP
	#ifdef ENV_WORLDPOS
		vec3 cameraToFrag;
		if ( isOrthographic ) {
			cameraToFrag = normalize( vec3( - viewMatrix[ 0 ][ 2 ], - viewMatrix[ 1 ][ 2 ], - viewMatrix[ 2 ][ 2 ] ) );
		} else {
			cameraToFrag = normalize( vWorldPosition - cameraPosition );
		}
		vec3 worldNormal = inverseTransformDirection( normal, viewMatrix );
		#ifdef ENVMAP_MODE_REFLECTION
			vec3 reflectVec = reflect( cameraToFrag, worldNormal );
		#else
			vec3 reflectVec = refract( cameraToFrag, worldNormal, refractionRatio );
		#endif
	#else
		vec3 reflectVec = vReflect;
	#endif
	#ifdef ENVMAP_TYPE_CUBE
		vec4 envColor = textureCube( envMap, envMapRotation * vec3( flipEnvMap * reflectVec.x, reflectVec.yz ) );
	#else
		vec4 envColor = vec4( 0.0 );
	#endif
	#ifdef ENVMAP_BLENDING_MULTIPLY
		outgoingLight = mix( outgoingLight, outgoingLight * envColor.xyz, specularStrength * reflectivity );
	#elif defined( ENVMAP_BLENDING_MIX )
		outgoingLight = mix( outgoingLight, envColor.xyz, specularStrength * reflectivity );
	#elif defined( ENVMAP_BLENDING_ADD )
		outgoingLight += envColor.xyz * specularStrength * reflectivity;
	#endif
#endif`,Pp=`#ifdef USE_ENVMAP
	uniform float envMapIntensity;
	uniform float flipEnvMap;
	uniform mat3 envMapRotation;
	#ifdef ENVMAP_TYPE_CUBE
		uniform samplerCube envMap;
	#else
		uniform sampler2D envMap;
	#endif
	
#endif`,Lp=`#ifdef USE_ENVMAP
	uniform float reflectivity;
	#if defined( USE_BUMPMAP ) || defined( USE_NORMALMAP ) || defined( PHONG ) || defined( LAMBERT )
		#define ENV_WORLDPOS
	#endif
	#ifdef ENV_WORLDPOS
		varying vec3 vWorldPosition;
		uniform float refractionRatio;
	#else
		varying vec3 vReflect;
	#endif
#endif`,Dp=`#ifdef USE_ENVMAP
	#if defined( USE_BUMPMAP ) || defined( USE_NORMALMAP ) || defined( PHONG ) || defined( LAMBERT )
		#define ENV_WORLDPOS
	#endif
	#ifdef ENV_WORLDPOS
		
		varying vec3 vWorldPosition;
	#else
		varying vec3 vReflect;
		uniform float refractionRatio;
	#endif
#endif`,Ip=`#ifdef USE_ENVMAP
	#ifdef ENV_WORLDPOS
		vWorldPosition = worldPosition.xyz;
	#else
		vec3 cameraToVertex;
		if ( isOrthographic ) {
			cameraToVertex = normalize( vec3( - viewMatrix[ 0 ][ 2 ], - viewMatrix[ 1 ][ 2 ], - viewMatrix[ 2 ][ 2 ] ) );
		} else {
			cameraToVertex = normalize( worldPosition.xyz - cameraPosition );
		}
		vec3 worldNormal = inverseTransformDirection( transformedNormal, viewMatrix );
		#ifdef ENVMAP_MODE_REFLECTION
			vReflect = reflect( cameraToVertex, worldNormal );
		#else
			vReflect = refract( cameraToVertex, worldNormal, refractionRatio );
		#endif
	#endif
#endif`,Up=`#ifdef USE_FOG
	vFogDepth = - mvPosition.z;
#endif`,kp=`#ifdef USE_FOG
	varying float vFogDepth;
#endif`,Np=`#ifdef USE_FOG
	#ifdef FOG_EXP2
		float fogFactor = 1.0 - exp( - fogDensity * fogDensity * vFogDepth * vFogDepth );
	#else
		float fogFactor = smoothstep( fogNear, fogFar, vFogDepth );
	#endif
	gl_FragColor.rgb = mix( gl_FragColor.rgb, fogColor, fogFactor );
#endif`,Fp=`#ifdef USE_FOG
	uniform vec3 fogColor;
	varying float vFogDepth;
	#ifdef FOG_EXP2
		uniform float fogDensity;
	#else
		uniform float fogNear;
		uniform float fogFar;
	#endif
#endif`,Op=`#ifdef USE_GRADIENTMAP
	uniform sampler2D gradientMap;
#endif
vec3 getGradientIrradiance( vec3 normal, vec3 lightDirection ) {
	float dotNL = dot( normal, lightDirection );
	vec2 coord = vec2( dotNL * 0.5 + 0.5, 0.0 );
	#ifdef USE_GRADIENTMAP
		return vec3( texture2D( gradientMap, coord ).r );
	#else
		vec2 fw = fwidth( coord ) * 0.5;
		return mix( vec3( 0.7 ), vec3( 1.0 ), smoothstep( 0.7 - fw.x, 0.7 + fw.x, coord.x ) );
	#endif
}`,Bp=`#ifdef USE_LIGHTMAP
	uniform sampler2D lightMap;
	uniform float lightMapIntensity;
#endif`,zp=`LambertMaterial material;
material.diffuseColor = diffuseColor.rgb;
material.specularStrength = specularStrength;`,Vp=`varying vec3 vViewPosition;
struct LambertMaterial {
	vec3 diffuseColor;
	float specularStrength;
};
void RE_Direct_Lambert( const in IncidentLight directLight, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in LambertMaterial material, inout ReflectedLight reflectedLight ) {
	float dotNL = saturate( dot( geometryNormal, directLight.direction ) );
	vec3 irradiance = dotNL * directLight.color;
	reflectedLight.directDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
}
void RE_IndirectDiffuse_Lambert( const in vec3 irradiance, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in LambertMaterial material, inout ReflectedLight reflectedLight ) {
	reflectedLight.indirectDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
}
#define RE_Direct				RE_Direct_Lambert
#define RE_IndirectDiffuse		RE_IndirectDiffuse_Lambert`,Hp=`uniform bool receiveShadow;
uniform vec3 ambientLightColor;
#if defined( USE_LIGHT_PROBES )
	uniform vec3 lightProbe[ 9 ];
#endif
vec3 shGetIrradianceAt( in vec3 normal, in vec3 shCoefficients[ 9 ] ) {
	float x = normal.x, y = normal.y, z = normal.z;
	vec3 result = shCoefficients[ 0 ] * 0.886227;
	result += shCoefficients[ 1 ] * 2.0 * 0.511664 * y;
	result += shCoefficients[ 2 ] * 2.0 * 0.511664 * z;
	result += shCoefficients[ 3 ] * 2.0 * 0.511664 * x;
	result += shCoefficients[ 4 ] * 2.0 * 0.429043 * x * y;
	result += shCoefficients[ 5 ] * 2.0 * 0.429043 * y * z;
	result += shCoefficients[ 6 ] * ( 0.743125 * z * z - 0.247708 );
	result += shCoefficients[ 7 ] * 2.0 * 0.429043 * x * z;
	result += shCoefficients[ 8 ] * 0.429043 * ( x * x - y * y );
	return result;
}
vec3 getLightProbeIrradiance( const in vec3 lightProbe[ 9 ], const in vec3 normal ) {
	vec3 worldNormal = inverseTransformDirection( normal, viewMatrix );
	vec3 irradiance = shGetIrradianceAt( worldNormal, lightProbe );
	return irradiance;
}
vec3 getAmbientLightIrradiance( const in vec3 ambientLightColor ) {
	vec3 irradiance = ambientLightColor;
	return irradiance;
}
float getDistanceAttenuation( const in float lightDistance, const in float cutoffDistance, const in float decayExponent ) {
	float distanceFalloff = 1.0 / max( pow( lightDistance, decayExponent ), 0.01 );
	if ( cutoffDistance > 0.0 ) {
		distanceFalloff *= pow2( saturate( 1.0 - pow4( lightDistance / cutoffDistance ) ) );
	}
	return distanceFalloff;
}
float getSpotAttenuation( const in float coneCosine, const in float penumbraCosine, const in float angleCosine ) {
	return smoothstep( coneCosine, penumbraCosine, angleCosine );
}
#if NUM_DIR_LIGHTS > 0
	struct DirectionalLight {
		vec3 direction;
		vec3 color;
	};
	uniform DirectionalLight directionalLights[ NUM_DIR_LIGHTS ];
	void getDirectionalLightInfo( const in DirectionalLight directionalLight, out IncidentLight light ) {
		light.color = directionalLight.color;
		light.direction = directionalLight.direction;
		light.visible = true;
	}
#endif
#if NUM_POINT_LIGHTS > 0
	struct PointLight {
		vec3 position;
		vec3 color;
		float distance;
		float decay;
	};
	uniform PointLight pointLights[ NUM_POINT_LIGHTS ];
	void getPointLightInfo( const in PointLight pointLight, const in vec3 geometryPosition, out IncidentLight light ) {
		vec3 lVector = pointLight.position - geometryPosition;
		light.direction = normalize( lVector );
		float lightDistance = length( lVector );
		light.color = pointLight.color;
		light.color *= getDistanceAttenuation( lightDistance, pointLight.distance, pointLight.decay );
		light.visible = ( light.color != vec3( 0.0 ) );
	}
#endif
#if NUM_SPOT_LIGHTS > 0
	struct SpotLight {
		vec3 position;
		vec3 direction;
		vec3 color;
		float distance;
		float decay;
		float coneCos;
		float penumbraCos;
	};
	uniform SpotLight spotLights[ NUM_SPOT_LIGHTS ];
	void getSpotLightInfo( const in SpotLight spotLight, const in vec3 geometryPosition, out IncidentLight light ) {
		vec3 lVector = spotLight.position - geometryPosition;
		light.direction = normalize( lVector );
		float angleCos = dot( light.direction, spotLight.direction );
		float spotAttenuation = getSpotAttenuation( spotLight.coneCos, spotLight.penumbraCos, angleCos );
		if ( spotAttenuation > 0.0 ) {
			float lightDistance = length( lVector );
			light.color = spotLight.color * spotAttenuation;
			light.color *= getDistanceAttenuation( lightDistance, spotLight.distance, spotLight.decay );
			light.visible = ( light.color != vec3( 0.0 ) );
		} else {
			light.color = vec3( 0.0 );
			light.visible = false;
		}
	}
#endif
#if NUM_RECT_AREA_LIGHTS > 0
	struct RectAreaLight {
		vec3 color;
		vec3 position;
		vec3 halfWidth;
		vec3 halfHeight;
	};
	uniform sampler2D ltc_1;	uniform sampler2D ltc_2;
	uniform RectAreaLight rectAreaLights[ NUM_RECT_AREA_LIGHTS ];
#endif
#if NUM_HEMI_LIGHTS > 0
	struct HemisphereLight {
		vec3 direction;
		vec3 skyColor;
		vec3 groundColor;
	};
	uniform HemisphereLight hemisphereLights[ NUM_HEMI_LIGHTS ];
	vec3 getHemisphereLightIrradiance( const in HemisphereLight hemiLight, const in vec3 normal ) {
		float dotNL = dot( normal, hemiLight.direction );
		float hemiDiffuseWeight = 0.5 * dotNL + 0.5;
		vec3 irradiance = mix( hemiLight.groundColor, hemiLight.skyColor, hemiDiffuseWeight );
		return irradiance;
	}
#endif`,Gp=`#ifdef USE_ENVMAP
	vec3 getIBLIrradiance( const in vec3 normal ) {
		#ifdef ENVMAP_TYPE_CUBE_UV
			vec3 worldNormal = inverseTransformDirection( normal, viewMatrix );
			vec4 envMapColor = textureCubeUV( envMap, envMapRotation * worldNormal, 1.0 );
			return PI * envMapColor.rgb * envMapIntensity;
		#else
			return vec3( 0.0 );
		#endif
	}
	vec3 getIBLRadiance( const in vec3 viewDir, const in vec3 normal, const in float roughness ) {
		#ifdef ENVMAP_TYPE_CUBE_UV
			vec3 reflectVec = reflect( - viewDir, normal );
			reflectVec = normalize( mix( reflectVec, normal, roughness * roughness) );
			reflectVec = inverseTransformDirection( reflectVec, viewMatrix );
			vec4 envMapColor = textureCubeUV( envMap, envMapRotation * reflectVec, roughness );
			return envMapColor.rgb * envMapIntensity;
		#else
			return vec3( 0.0 );
		#endif
	}
	#ifdef USE_ANISOTROPY
		vec3 getIBLAnisotropyRadiance( const in vec3 viewDir, const in vec3 normal, const in float roughness, const in vec3 bitangent, const in float anisotropy ) {
			#ifdef ENVMAP_TYPE_CUBE_UV
				vec3 bentNormal = cross( bitangent, viewDir );
				bentNormal = normalize( cross( bentNormal, bitangent ) );
				bentNormal = normalize( mix( bentNormal, normal, pow2( pow2( 1.0 - anisotropy * ( 1.0 - roughness ) ) ) ) );
				return getIBLRadiance( viewDir, bentNormal, roughness );
			#else
				return vec3( 0.0 );
			#endif
		}
	#endif
#endif`,Wp=`ToonMaterial material;
material.diffuseColor = diffuseColor.rgb;`,$p=`varying vec3 vViewPosition;
struct ToonMaterial {
	vec3 diffuseColor;
};
void RE_Direct_Toon( const in IncidentLight directLight, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in ToonMaterial material, inout ReflectedLight reflectedLight ) {
	vec3 irradiance = getGradientIrradiance( geometryNormal, directLight.direction ) * directLight.color;
	reflectedLight.directDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
}
void RE_IndirectDiffuse_Toon( const in vec3 irradiance, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in ToonMaterial material, inout ReflectedLight reflectedLight ) {
	reflectedLight.indirectDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
}
#define RE_Direct				RE_Direct_Toon
#define RE_IndirectDiffuse		RE_IndirectDiffuse_Toon`,Xp=`BlinnPhongMaterial material;
material.diffuseColor = diffuseColor.rgb;
material.specularColor = specular;
material.specularShininess = shininess;
material.specularStrength = specularStrength;`,qp=`varying vec3 vViewPosition;
struct BlinnPhongMaterial {
	vec3 diffuseColor;
	vec3 specularColor;
	float specularShininess;
	float specularStrength;
};
void RE_Direct_BlinnPhong( const in IncidentLight directLight, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in BlinnPhongMaterial material, inout ReflectedLight reflectedLight ) {
	float dotNL = saturate( dot( geometryNormal, directLight.direction ) );
	vec3 irradiance = dotNL * directLight.color;
	reflectedLight.directDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
	reflectedLight.directSpecular += irradiance * BRDF_BlinnPhong( directLight.direction, geometryViewDir, geometryNormal, material.specularColor, material.specularShininess ) * material.specularStrength;
}
void RE_IndirectDiffuse_BlinnPhong( const in vec3 irradiance, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in BlinnPhongMaterial material, inout ReflectedLight reflectedLight ) {
	reflectedLight.indirectDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
}
#define RE_Direct				RE_Direct_BlinnPhong
#define RE_IndirectDiffuse		RE_IndirectDiffuse_BlinnPhong`,Yp=`PhysicalMaterial material;
material.diffuseColor = diffuseColor.rgb * ( 1.0 - metalnessFactor );
vec3 dxy = max( abs( dFdx( nonPerturbedNormal ) ), abs( dFdy( nonPerturbedNormal ) ) );
float geometryRoughness = max( max( dxy.x, dxy.y ), dxy.z );
material.roughness = max( roughnessFactor, 0.0525 );material.roughness += geometryRoughness;
material.roughness = min( material.roughness, 1.0 );
#ifdef IOR
	material.ior = ior;
	#ifdef USE_SPECULAR
		float specularIntensityFactor = specularIntensity;
		vec3 specularColorFactor = specularColor;
		#ifdef USE_SPECULAR_COLORMAP
			specularColorFactor *= texture2D( specularColorMap, vSpecularColorMapUv ).rgb;
		#endif
		#ifdef USE_SPECULAR_INTENSITYMAP
			specularIntensityFactor *= texture2D( specularIntensityMap, vSpecularIntensityMapUv ).a;
		#endif
		material.specularF90 = mix( specularIntensityFactor, 1.0, metalnessFactor );
	#else
		float specularIntensityFactor = 1.0;
		vec3 specularColorFactor = vec3( 1.0 );
		material.specularF90 = 1.0;
	#endif
	material.specularColor = mix( min( pow2( ( material.ior - 1.0 ) / ( material.ior + 1.0 ) ) * specularColorFactor, vec3( 1.0 ) ) * specularIntensityFactor, diffuseColor.rgb, metalnessFactor );
#else
	material.specularColor = mix( vec3( 0.04 ), diffuseColor.rgb, metalnessFactor );
	material.specularF90 = 1.0;
#endif
#ifdef USE_CLEARCOAT
	material.clearcoat = clearcoat;
	material.clearcoatRoughness = clearcoatRoughness;
	material.clearcoatF0 = vec3( 0.04 );
	material.clearcoatF90 = 1.0;
	#ifdef USE_CLEARCOATMAP
		material.clearcoat *= texture2D( clearcoatMap, vClearcoatMapUv ).x;
	#endif
	#ifdef USE_CLEARCOAT_ROUGHNESSMAP
		material.clearcoatRoughness *= texture2D( clearcoatRoughnessMap, vClearcoatRoughnessMapUv ).y;
	#endif
	material.clearcoat = saturate( material.clearcoat );	material.clearcoatRoughness = max( material.clearcoatRoughness, 0.0525 );
	material.clearcoatRoughness += geometryRoughness;
	material.clearcoatRoughness = min( material.clearcoatRoughness, 1.0 );
#endif
#ifdef USE_DISPERSION
	material.dispersion = dispersion;
#endif
#ifdef USE_IRIDESCENCE
	material.iridescence = iridescence;
	material.iridescenceIOR = iridescenceIOR;
	#ifdef USE_IRIDESCENCEMAP
		material.iridescence *= texture2D( iridescenceMap, vIridescenceMapUv ).r;
	#endif
	#ifdef USE_IRIDESCENCE_THICKNESSMAP
		material.iridescenceThickness = (iridescenceThicknessMaximum - iridescenceThicknessMinimum) * texture2D( iridescenceThicknessMap, vIridescenceThicknessMapUv ).g + iridescenceThicknessMinimum;
	#else
		material.iridescenceThickness = iridescenceThicknessMaximum;
	#endif
#endif
#ifdef USE_SHEEN
	material.sheenColor = sheenColor;
	#ifdef USE_SHEEN_COLORMAP
		material.sheenColor *= texture2D( sheenColorMap, vSheenColorMapUv ).rgb;
	#endif
	material.sheenRoughness = clamp( sheenRoughness, 0.07, 1.0 );
	#ifdef USE_SHEEN_ROUGHNESSMAP
		material.sheenRoughness *= texture2D( sheenRoughnessMap, vSheenRoughnessMapUv ).a;
	#endif
#endif
#ifdef USE_ANISOTROPY
	#ifdef USE_ANISOTROPYMAP
		mat2 anisotropyMat = mat2( anisotropyVector.x, anisotropyVector.y, - anisotropyVector.y, anisotropyVector.x );
		vec3 anisotropyPolar = texture2D( anisotropyMap, vAnisotropyMapUv ).rgb;
		vec2 anisotropyV = anisotropyMat * normalize( 2.0 * anisotropyPolar.rg - vec2( 1.0 ) ) * anisotropyPolar.b;
	#else
		vec2 anisotropyV = anisotropyVector;
	#endif
	material.anisotropy = length( anisotropyV );
	if( material.anisotropy == 0.0 ) {
		anisotropyV = vec2( 1.0, 0.0 );
	} else {
		anisotropyV /= material.anisotropy;
		material.anisotropy = saturate( material.anisotropy );
	}
	material.alphaT = mix( pow2( material.roughness ), 1.0, pow2( material.anisotropy ) );
	material.anisotropyT = tbn[ 0 ] * anisotropyV.x + tbn[ 1 ] * anisotropyV.y;
	material.anisotropyB = tbn[ 1 ] * anisotropyV.x - tbn[ 0 ] * anisotropyV.y;
#endif`,jp=`struct PhysicalMaterial {
	vec3 diffuseColor;
	float roughness;
	vec3 specularColor;
	float specularF90;
	float dispersion;
	#ifdef USE_CLEARCOAT
		float clearcoat;
		float clearcoatRoughness;
		vec3 clearcoatF0;
		float clearcoatF90;
	#endif
	#ifdef USE_IRIDESCENCE
		float iridescence;
		float iridescenceIOR;
		float iridescenceThickness;
		vec3 iridescenceFresnel;
		vec3 iridescenceF0;
	#endif
	#ifdef USE_SHEEN
		vec3 sheenColor;
		float sheenRoughness;
	#endif
	#ifdef IOR
		float ior;
	#endif
	#ifdef USE_TRANSMISSION
		float transmission;
		float transmissionAlpha;
		float thickness;
		float attenuationDistance;
		vec3 attenuationColor;
	#endif
	#ifdef USE_ANISOTROPY
		float anisotropy;
		float alphaT;
		vec3 anisotropyT;
		vec3 anisotropyB;
	#endif
};
vec3 clearcoatSpecularDirect = vec3( 0.0 );
vec3 clearcoatSpecularIndirect = vec3( 0.0 );
vec3 sheenSpecularDirect = vec3( 0.0 );
vec3 sheenSpecularIndirect = vec3(0.0 );
vec3 Schlick_to_F0( const in vec3 f, const in float f90, const in float dotVH ) {
    float x = clamp( 1.0 - dotVH, 0.0, 1.0 );
    float x2 = x * x;
    float x5 = clamp( x * x2 * x2, 0.0, 0.9999 );
    return ( f - vec3( f90 ) * x5 ) / ( 1.0 - x5 );
}
float V_GGX_SmithCorrelated( const in float alpha, const in float dotNL, const in float dotNV ) {
	float a2 = pow2( alpha );
	float gv = dotNL * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNV ) );
	float gl = dotNV * sqrt( a2 + ( 1.0 - a2 ) * pow2( dotNL ) );
	return 0.5 / max( gv + gl, EPSILON );
}
float D_GGX( const in float alpha, const in float dotNH ) {
	float a2 = pow2( alpha );
	float denom = pow2( dotNH ) * ( a2 - 1.0 ) + 1.0;
	return RECIPROCAL_PI * a2 / pow2( denom );
}
#ifdef USE_ANISOTROPY
	float V_GGX_SmithCorrelated_Anisotropic( const in float alphaT, const in float alphaB, const in float dotTV, const in float dotBV, const in float dotTL, const in float dotBL, const in float dotNV, const in float dotNL ) {
		float gv = dotNL * length( vec3( alphaT * dotTV, alphaB * dotBV, dotNV ) );
		float gl = dotNV * length( vec3( alphaT * dotTL, alphaB * dotBL, dotNL ) );
		float v = 0.5 / ( gv + gl );
		return saturate(v);
	}
	float D_GGX_Anisotropic( const in float alphaT, const in float alphaB, const in float dotNH, const in float dotTH, const in float dotBH ) {
		float a2 = alphaT * alphaB;
		highp vec3 v = vec3( alphaB * dotTH, alphaT * dotBH, a2 * dotNH );
		highp float v2 = dot( v, v );
		float w2 = a2 / v2;
		return RECIPROCAL_PI * a2 * pow2 ( w2 );
	}
#endif
#ifdef USE_CLEARCOAT
	vec3 BRDF_GGX_Clearcoat( const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal, const in PhysicalMaterial material) {
		vec3 f0 = material.clearcoatF0;
		float f90 = material.clearcoatF90;
		float roughness = material.clearcoatRoughness;
		float alpha = pow2( roughness );
		vec3 halfDir = normalize( lightDir + viewDir );
		float dotNL = saturate( dot( normal, lightDir ) );
		float dotNV = saturate( dot( normal, viewDir ) );
		float dotNH = saturate( dot( normal, halfDir ) );
		float dotVH = saturate( dot( viewDir, halfDir ) );
		vec3 F = F_Schlick( f0, f90, dotVH );
		float V = V_GGX_SmithCorrelated( alpha, dotNL, dotNV );
		float D = D_GGX( alpha, dotNH );
		return F * ( V * D );
	}
#endif
vec3 BRDF_GGX( const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal, const in PhysicalMaterial material ) {
	vec3 f0 = material.specularColor;
	float f90 = material.specularF90;
	float roughness = material.roughness;
	float alpha = pow2( roughness );
	vec3 halfDir = normalize( lightDir + viewDir );
	float dotNL = saturate( dot( normal, lightDir ) );
	float dotNV = saturate( dot( normal, viewDir ) );
	float dotNH = saturate( dot( normal, halfDir ) );
	float dotVH = saturate( dot( viewDir, halfDir ) );
	vec3 F = F_Schlick( f0, f90, dotVH );
	#ifdef USE_IRIDESCENCE
		F = mix( F, material.iridescenceFresnel, material.iridescence );
	#endif
	#ifdef USE_ANISOTROPY
		float dotTL = dot( material.anisotropyT, lightDir );
		float dotTV = dot( material.anisotropyT, viewDir );
		float dotTH = dot( material.anisotropyT, halfDir );
		float dotBL = dot( material.anisotropyB, lightDir );
		float dotBV = dot( material.anisotropyB, viewDir );
		float dotBH = dot( material.anisotropyB, halfDir );
		float V = V_GGX_SmithCorrelated_Anisotropic( material.alphaT, alpha, dotTV, dotBV, dotTL, dotBL, dotNV, dotNL );
		float D = D_GGX_Anisotropic( material.alphaT, alpha, dotNH, dotTH, dotBH );
	#else
		float V = V_GGX_SmithCorrelated( alpha, dotNL, dotNV );
		float D = D_GGX( alpha, dotNH );
	#endif
	return F * ( V * D );
}
vec2 LTC_Uv( const in vec3 N, const in vec3 V, const in float roughness ) {
	const float LUT_SIZE = 64.0;
	const float LUT_SCALE = ( LUT_SIZE - 1.0 ) / LUT_SIZE;
	const float LUT_BIAS = 0.5 / LUT_SIZE;
	float dotNV = saturate( dot( N, V ) );
	vec2 uv = vec2( roughness, sqrt( 1.0 - dotNV ) );
	uv = uv * LUT_SCALE + LUT_BIAS;
	return uv;
}
float LTC_ClippedSphereFormFactor( const in vec3 f ) {
	float l = length( f );
	return max( ( l * l + f.z ) / ( l + 1.0 ), 0.0 );
}
vec3 LTC_EdgeVectorFormFactor( const in vec3 v1, const in vec3 v2 ) {
	float x = dot( v1, v2 );
	float y = abs( x );
	float a = 0.8543985 + ( 0.4965155 + 0.0145206 * y ) * y;
	float b = 3.4175940 + ( 4.1616724 + y ) * y;
	float v = a / b;
	float theta_sintheta = ( x > 0.0 ) ? v : 0.5 * inversesqrt( max( 1.0 - x * x, 1e-7 ) ) - v;
	return cross( v1, v2 ) * theta_sintheta;
}
vec3 LTC_Evaluate( const in vec3 N, const in vec3 V, const in vec3 P, const in mat3 mInv, const in vec3 rectCoords[ 4 ] ) {
	vec3 v1 = rectCoords[ 1 ] - rectCoords[ 0 ];
	vec3 v2 = rectCoords[ 3 ] - rectCoords[ 0 ];
	vec3 lightNormal = cross( v1, v2 );
	if( dot( lightNormal, P - rectCoords[ 0 ] ) < 0.0 ) return vec3( 0.0 );
	vec3 T1, T2;
	T1 = normalize( V - N * dot( V, N ) );
	T2 = - cross( N, T1 );
	mat3 mat = mInv * transposeMat3( mat3( T1, T2, N ) );
	vec3 coords[ 4 ];
	coords[ 0 ] = mat * ( rectCoords[ 0 ] - P );
	coords[ 1 ] = mat * ( rectCoords[ 1 ] - P );
	coords[ 2 ] = mat * ( rectCoords[ 2 ] - P );
	coords[ 3 ] = mat * ( rectCoords[ 3 ] - P );
	coords[ 0 ] = normalize( coords[ 0 ] );
	coords[ 1 ] = normalize( coords[ 1 ] );
	coords[ 2 ] = normalize( coords[ 2 ] );
	coords[ 3 ] = normalize( coords[ 3 ] );
	vec3 vectorFormFactor = vec3( 0.0 );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 0 ], coords[ 1 ] );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 1 ], coords[ 2 ] );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 2 ], coords[ 3 ] );
	vectorFormFactor += LTC_EdgeVectorFormFactor( coords[ 3 ], coords[ 0 ] );
	float result = LTC_ClippedSphereFormFactor( vectorFormFactor );
	return vec3( result );
}
#if defined( USE_SHEEN )
float D_Charlie( float roughness, float dotNH ) {
	float alpha = pow2( roughness );
	float invAlpha = 1.0 / alpha;
	float cos2h = dotNH * dotNH;
	float sin2h = max( 1.0 - cos2h, 0.0078125 );
	return ( 2.0 + invAlpha ) * pow( sin2h, invAlpha * 0.5 ) / ( 2.0 * PI );
}
float V_Neubelt( float dotNV, float dotNL ) {
	return saturate( 1.0 / ( 4.0 * ( dotNL + dotNV - dotNL * dotNV ) ) );
}
vec3 BRDF_Sheen( const in vec3 lightDir, const in vec3 viewDir, const in vec3 normal, vec3 sheenColor, const in float sheenRoughness ) {
	vec3 halfDir = normalize( lightDir + viewDir );
	float dotNL = saturate( dot( normal, lightDir ) );
	float dotNV = saturate( dot( normal, viewDir ) );
	float dotNH = saturate( dot( normal, halfDir ) );
	float D = D_Charlie( sheenRoughness, dotNH );
	float V = V_Neubelt( dotNV, dotNL );
	return sheenColor * ( D * V );
}
#endif
float IBLSheenBRDF( const in vec3 normal, const in vec3 viewDir, const in float roughness ) {
	float dotNV = saturate( dot( normal, viewDir ) );
	float r2 = roughness * roughness;
	float a = roughness < 0.25 ? -339.2 * r2 + 161.4 * roughness - 25.9 : -8.48 * r2 + 14.3 * roughness - 9.95;
	float b = roughness < 0.25 ? 44.0 * r2 - 23.7 * roughness + 3.26 : 1.97 * r2 - 3.27 * roughness + 0.72;
	float DG = exp( a * dotNV + b ) + ( roughness < 0.25 ? 0.0 : 0.1 * ( roughness - 0.25 ) );
	return saturate( DG * RECIPROCAL_PI );
}
vec2 DFGApprox( const in vec3 normal, const in vec3 viewDir, const in float roughness ) {
	float dotNV = saturate( dot( normal, viewDir ) );
	const vec4 c0 = vec4( - 1, - 0.0275, - 0.572, 0.022 );
	const vec4 c1 = vec4( 1, 0.0425, 1.04, - 0.04 );
	vec4 r = roughness * c0 + c1;
	float a004 = min( r.x * r.x, exp2( - 9.28 * dotNV ) ) * r.x + r.y;
	vec2 fab = vec2( - 1.04, 1.04 ) * a004 + r.zw;
	return fab;
}
vec3 EnvironmentBRDF( const in vec3 normal, const in vec3 viewDir, const in vec3 specularColor, const in float specularF90, const in float roughness ) {
	vec2 fab = DFGApprox( normal, viewDir, roughness );
	return specularColor * fab.x + specularF90 * fab.y;
}
#ifdef USE_IRIDESCENCE
void computeMultiscatteringIridescence( const in vec3 normal, const in vec3 viewDir, const in vec3 specularColor, const in float specularF90, const in float iridescence, const in vec3 iridescenceF0, const in float roughness, inout vec3 singleScatter, inout vec3 multiScatter ) {
#else
void computeMultiscattering( const in vec3 normal, const in vec3 viewDir, const in vec3 specularColor, const in float specularF90, const in float roughness, inout vec3 singleScatter, inout vec3 multiScatter ) {
#endif
	vec2 fab = DFGApprox( normal, viewDir, roughness );
	#ifdef USE_IRIDESCENCE
		vec3 Fr = mix( specularColor, iridescenceF0, iridescence );
	#else
		vec3 Fr = specularColor;
	#endif
	vec3 FssEss = Fr * fab.x + specularF90 * fab.y;
	float Ess = fab.x + fab.y;
	float Ems = 1.0 - Ess;
	vec3 Favg = Fr + ( 1.0 - Fr ) * 0.047619;	vec3 Fms = FssEss * Favg / ( 1.0 - Ems * Favg );
	singleScatter += FssEss;
	multiScatter += Fms * Ems;
}
#if NUM_RECT_AREA_LIGHTS > 0
	void RE_Direct_RectArea_Physical( const in RectAreaLight rectAreaLight, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {
		vec3 normal = geometryNormal;
		vec3 viewDir = geometryViewDir;
		vec3 position = geometryPosition;
		vec3 lightPos = rectAreaLight.position;
		vec3 halfWidth = rectAreaLight.halfWidth;
		vec3 halfHeight = rectAreaLight.halfHeight;
		vec3 lightColor = rectAreaLight.color;
		float roughness = material.roughness;
		vec3 rectCoords[ 4 ];
		rectCoords[ 0 ] = lightPos + halfWidth - halfHeight;		rectCoords[ 1 ] = lightPos - halfWidth - halfHeight;
		rectCoords[ 2 ] = lightPos - halfWidth + halfHeight;
		rectCoords[ 3 ] = lightPos + halfWidth + halfHeight;
		vec2 uv = LTC_Uv( normal, viewDir, roughness );
		vec4 t1 = texture2D( ltc_1, uv );
		vec4 t2 = texture2D( ltc_2, uv );
		mat3 mInv = mat3(
			vec3( t1.x, 0, t1.y ),
			vec3(    0, 1,    0 ),
			vec3( t1.z, 0, t1.w )
		);
		vec3 fresnel = ( material.specularColor * t2.x + ( vec3( 1.0 ) - material.specularColor ) * t2.y );
		reflectedLight.directSpecular += lightColor * fresnel * LTC_Evaluate( normal, viewDir, position, mInv, rectCoords );
		reflectedLight.directDiffuse += lightColor * material.diffuseColor * LTC_Evaluate( normal, viewDir, position, mat3( 1.0 ), rectCoords );
	}
#endif
void RE_Direct_Physical( const in IncidentLight directLight, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {
	float dotNL = saturate( dot( geometryNormal, directLight.direction ) );
	vec3 irradiance = dotNL * directLight.color;
	#ifdef USE_CLEARCOAT
		float dotNLcc = saturate( dot( geometryClearcoatNormal, directLight.direction ) );
		vec3 ccIrradiance = dotNLcc * directLight.color;
		clearcoatSpecularDirect += ccIrradiance * BRDF_GGX_Clearcoat( directLight.direction, geometryViewDir, geometryClearcoatNormal, material );
	#endif
	#ifdef USE_SHEEN
		sheenSpecularDirect += irradiance * BRDF_Sheen( directLight.direction, geometryViewDir, geometryNormal, material.sheenColor, material.sheenRoughness );
	#endif
	reflectedLight.directSpecular += irradiance * BRDF_GGX( directLight.direction, geometryViewDir, geometryNormal, material );
	reflectedLight.directDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
}
void RE_IndirectDiffuse_Physical( const in vec3 irradiance, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in PhysicalMaterial material, inout ReflectedLight reflectedLight ) {
	reflectedLight.indirectDiffuse += irradiance * BRDF_Lambert( material.diffuseColor );
}
void RE_IndirectSpecular_Physical( const in vec3 radiance, const in vec3 irradiance, const in vec3 clearcoatRadiance, const in vec3 geometryPosition, const in vec3 geometryNormal, const in vec3 geometryViewDir, const in vec3 geometryClearcoatNormal, const in PhysicalMaterial material, inout ReflectedLight reflectedLight) {
	#ifdef USE_CLEARCOAT
		clearcoatSpecularIndirect += clearcoatRadiance * EnvironmentBRDF( geometryClearcoatNormal, geometryViewDir, material.clearcoatF0, material.clearcoatF90, material.clearcoatRoughness );
	#endif
	#ifdef USE_SHEEN
		sheenSpecularIndirect += irradiance * material.sheenColor * IBLSheenBRDF( geometryNormal, geometryViewDir, material.sheenRoughness );
	#endif
	vec3 singleScattering = vec3( 0.0 );
	vec3 multiScattering = vec3( 0.0 );
	vec3 cosineWeightedIrradiance = irradiance * RECIPROCAL_PI;
	#ifdef USE_IRIDESCENCE
		computeMultiscatteringIridescence( geometryNormal, geometryViewDir, material.specularColor, material.specularF90, material.iridescence, material.iridescenceFresnel, material.roughness, singleScattering, multiScattering );
	#else
		computeMultiscattering( geometryNormal, geometryViewDir, material.specularColor, material.specularF90, material.roughness, singleScattering, multiScattering );
	#endif
	vec3 totalScattering = singleScattering + multiScattering;
	vec3 diffuse = material.diffuseColor * ( 1.0 - max( max( totalScattering.r, totalScattering.g ), totalScattering.b ) );
	reflectedLight.indirectSpecular += radiance * singleScattering;
	reflectedLight.indirectSpecular += multiScattering * cosineWeightedIrradiance;
	reflectedLight.indirectDiffuse += diffuse * cosineWeightedIrradiance;
}
#define RE_Direct				RE_Direct_Physical
#define RE_Direct_RectArea		RE_Direct_RectArea_Physical
#define RE_IndirectDiffuse		RE_IndirectDiffuse_Physical
#define RE_IndirectSpecular		RE_IndirectSpecular_Physical
float computeSpecularOcclusion( const in float dotNV, const in float ambientOcclusion, const in float roughness ) {
	return saturate( pow( dotNV + ambientOcclusion, exp2( - 16.0 * roughness - 1.0 ) ) - 1.0 + ambientOcclusion );
}`,Zp=`
vec3 geometryPosition = - vViewPosition;
vec3 geometryNormal = normal;
vec3 geometryViewDir = ( isOrthographic ) ? vec3( 0, 0, 1 ) : normalize( vViewPosition );
vec3 geometryClearcoatNormal = vec3( 0.0 );
#ifdef USE_CLEARCOAT
	geometryClearcoatNormal = clearcoatNormal;
#endif
#ifdef USE_IRIDESCENCE
	float dotNVi = saturate( dot( normal, geometryViewDir ) );
	if ( material.iridescenceThickness == 0.0 ) {
		material.iridescence = 0.0;
	} else {
		material.iridescence = saturate( material.iridescence );
	}
	if ( material.iridescence > 0.0 ) {
		material.iridescenceFresnel = evalIridescence( 1.0, material.iridescenceIOR, dotNVi, material.iridescenceThickness, material.specularColor );
		material.iridescenceF0 = Schlick_to_F0( material.iridescenceFresnel, 1.0, dotNVi );
	}
#endif
IncidentLight directLight;
#if ( NUM_POINT_LIGHTS > 0 ) && defined( RE_Direct )
	PointLight pointLight;
	#if defined( USE_SHADOWMAP ) && NUM_POINT_LIGHT_SHADOWS > 0
	PointLightShadow pointLightShadow;
	#endif
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_POINT_LIGHTS; i ++ ) {
		pointLight = pointLights[ i ];
		getPointLightInfo( pointLight, geometryPosition, directLight );
		#if defined( USE_SHADOWMAP ) && ( UNROLLED_LOOP_INDEX < NUM_POINT_LIGHT_SHADOWS )
		pointLightShadow = pointLightShadows[ i ];
		directLight.color *= ( directLight.visible && receiveShadow ) ? getPointShadow( pointShadowMap[ i ], pointLightShadow.shadowMapSize, pointLightShadow.shadowIntensity, pointLightShadow.shadowBias, pointLightShadow.shadowRadius, vPointShadowCoord[ i ], pointLightShadow.shadowCameraNear, pointLightShadow.shadowCameraFar ) : 1.0;
		#endif
		RE_Direct( directLight, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, material, reflectedLight );
	}
	#pragma unroll_loop_end
#endif
#if ( NUM_SPOT_LIGHTS > 0 ) && defined( RE_Direct )
	SpotLight spotLight;
	vec4 spotColor;
	vec3 spotLightCoord;
	bool inSpotLightMap;
	#if defined( USE_SHADOWMAP ) && NUM_SPOT_LIGHT_SHADOWS > 0
	SpotLightShadow spotLightShadow;
	#endif
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_SPOT_LIGHTS; i ++ ) {
		spotLight = spotLights[ i ];
		getSpotLightInfo( spotLight, geometryPosition, directLight );
		#if ( UNROLLED_LOOP_INDEX < NUM_SPOT_LIGHT_SHADOWS_WITH_MAPS )
		#define SPOT_LIGHT_MAP_INDEX UNROLLED_LOOP_INDEX
		#elif ( UNROLLED_LOOP_INDEX < NUM_SPOT_LIGHT_SHADOWS )
		#define SPOT_LIGHT_MAP_INDEX NUM_SPOT_LIGHT_MAPS
		#else
		#define SPOT_LIGHT_MAP_INDEX ( UNROLLED_LOOP_INDEX - NUM_SPOT_LIGHT_SHADOWS + NUM_SPOT_LIGHT_SHADOWS_WITH_MAPS )
		#endif
		#if ( SPOT_LIGHT_MAP_INDEX < NUM_SPOT_LIGHT_MAPS )
			spotLightCoord = vSpotLightCoord[ i ].xyz / vSpotLightCoord[ i ].w;
			inSpotLightMap = all( lessThan( abs( spotLightCoord * 2. - 1. ), vec3( 1.0 ) ) );
			spotColor = texture2D( spotLightMap[ SPOT_LIGHT_MAP_INDEX ], spotLightCoord.xy );
			directLight.color = inSpotLightMap ? directLight.color * spotColor.rgb : directLight.color;
		#endif
		#undef SPOT_LIGHT_MAP_INDEX
		#if defined( USE_SHADOWMAP ) && ( UNROLLED_LOOP_INDEX < NUM_SPOT_LIGHT_SHADOWS )
		spotLightShadow = spotLightShadows[ i ];
		directLight.color *= ( directLight.visible && receiveShadow ) ? getShadow( spotShadowMap[ i ], spotLightShadow.shadowMapSize, spotLightShadow.shadowIntensity, spotLightShadow.shadowBias, spotLightShadow.shadowRadius, vSpotLightCoord[ i ] ) : 1.0;
		#endif
		RE_Direct( directLight, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, material, reflectedLight );
	}
	#pragma unroll_loop_end
#endif
#if ( NUM_DIR_LIGHTS > 0 ) && defined( RE_Direct )
	DirectionalLight directionalLight;
	#if defined( USE_SHADOWMAP ) && NUM_DIR_LIGHT_SHADOWS > 0
	DirectionalLightShadow directionalLightShadow;
	#endif
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_DIR_LIGHTS; i ++ ) {
		directionalLight = directionalLights[ i ];
		getDirectionalLightInfo( directionalLight, directLight );
		#if defined( USE_SHADOWMAP ) && ( UNROLLED_LOOP_INDEX < NUM_DIR_LIGHT_SHADOWS )
		directionalLightShadow = directionalLightShadows[ i ];
		directLight.color *= ( directLight.visible && receiveShadow ) ? getShadow( directionalShadowMap[ i ], directionalLightShadow.shadowMapSize, directionalLightShadow.shadowIntensity, directionalLightShadow.shadowBias, directionalLightShadow.shadowRadius, vDirectionalShadowCoord[ i ] ) : 1.0;
		#endif
		RE_Direct( directLight, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, material, reflectedLight );
	}
	#pragma unroll_loop_end
#endif
#if ( NUM_RECT_AREA_LIGHTS > 0 ) && defined( RE_Direct_RectArea )
	RectAreaLight rectAreaLight;
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_RECT_AREA_LIGHTS; i ++ ) {
		rectAreaLight = rectAreaLights[ i ];
		RE_Direct_RectArea( rectAreaLight, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, material, reflectedLight );
	}
	#pragma unroll_loop_end
#endif
#if defined( RE_IndirectDiffuse )
	vec3 iblIrradiance = vec3( 0.0 );
	vec3 irradiance = getAmbientLightIrradiance( ambientLightColor );
	#if defined( USE_LIGHT_PROBES )
		irradiance += getLightProbeIrradiance( lightProbe, geometryNormal );
	#endif
	#if ( NUM_HEMI_LIGHTS > 0 )
		#pragma unroll_loop_start
		for ( int i = 0; i < NUM_HEMI_LIGHTS; i ++ ) {
			irradiance += getHemisphereLightIrradiance( hemisphereLights[ i ], geometryNormal );
		}
		#pragma unroll_loop_end
	#endif
#endif
#if defined( RE_IndirectSpecular )
	vec3 radiance = vec3( 0.0 );
	vec3 clearcoatRadiance = vec3( 0.0 );
#endif`,Kp=`#if defined( RE_IndirectDiffuse )
	#ifdef USE_LIGHTMAP
		vec4 lightMapTexel = texture2D( lightMap, vLightMapUv );
		vec3 lightMapIrradiance = lightMapTexel.rgb * lightMapIntensity;
		irradiance += lightMapIrradiance;
	#endif
	#if defined( USE_ENVMAP ) && defined( STANDARD ) && defined( ENVMAP_TYPE_CUBE_UV )
		iblIrradiance += getIBLIrradiance( geometryNormal );
	#endif
#endif
#if defined( USE_ENVMAP ) && defined( RE_IndirectSpecular )
	#ifdef USE_ANISOTROPY
		radiance += getIBLAnisotropyRadiance( geometryViewDir, geometryNormal, material.roughness, material.anisotropyB, material.anisotropy );
	#else
		radiance += getIBLRadiance( geometryViewDir, geometryNormal, material.roughness );
	#endif
	#ifdef USE_CLEARCOAT
		clearcoatRadiance += getIBLRadiance( geometryViewDir, geometryClearcoatNormal, material.clearcoatRoughness );
	#endif
#endif`,Jp=`#if defined( RE_IndirectDiffuse )
	RE_IndirectDiffuse( irradiance, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, material, reflectedLight );
#endif
#if defined( RE_IndirectSpecular )
	RE_IndirectSpecular( radiance, iblIrradiance, clearcoatRadiance, geometryPosition, geometryNormal, geometryViewDir, geometryClearcoatNormal, material, reflectedLight );
#endif`,Qp=`#if defined( USE_LOGDEPTHBUF )
	gl_FragDepth = vIsPerspective == 0.0 ? gl_FragCoord.z : log2( vFragDepth ) * logDepthBufFC * 0.5;
#endif`,em=`#if defined( USE_LOGDEPTHBUF )
	uniform float logDepthBufFC;
	varying float vFragDepth;
	varying float vIsPerspective;
#endif`,tm=`#ifdef USE_LOGDEPTHBUF
	varying float vFragDepth;
	varying float vIsPerspective;
#endif`,nm=`#ifdef USE_LOGDEPTHBUF
	vFragDepth = 1.0 + gl_Position.w;
	vIsPerspective = float( isPerspectiveMatrix( projectionMatrix ) );
#endif`,im=`#ifdef USE_MAP
	vec4 sampledDiffuseColor = texture2D( map, vMapUv );
	#ifdef DECODE_VIDEO_TEXTURE
		sampledDiffuseColor = sRGBTransferEOTF( sampledDiffuseColor );
	#endif
	diffuseColor *= sampledDiffuseColor;
#endif`,sm=`#ifdef USE_MAP
	uniform sampler2D map;
#endif`,rm=`#if defined( USE_MAP ) || defined( USE_ALPHAMAP )
	#if defined( USE_POINTS_UV )
		vec2 uv = vUv;
	#else
		vec2 uv = ( uvTransform * vec3( gl_PointCoord.x, 1.0 - gl_PointCoord.y, 1 ) ).xy;
	#endif
#endif
#ifdef USE_MAP
	diffuseColor *= texture2D( map, uv );
#endif
#ifdef USE_ALPHAMAP
	diffuseColor.a *= texture2D( alphaMap, uv ).g;
#endif`,am=`#if defined( USE_POINTS_UV )
	varying vec2 vUv;
#else
	#if defined( USE_MAP ) || defined( USE_ALPHAMAP )
		uniform mat3 uvTransform;
	#endif
#endif
#ifdef USE_MAP
	uniform sampler2D map;
#endif
#ifdef USE_ALPHAMAP
	uniform sampler2D alphaMap;
#endif`,om=`float metalnessFactor = metalness;
#ifdef USE_METALNESSMAP
	vec4 texelMetalness = texture2D( metalnessMap, vMetalnessMapUv );
	metalnessFactor *= texelMetalness.b;
#endif`,lm=`#ifdef USE_METALNESSMAP
	uniform sampler2D metalnessMap;
#endif`,cm=`#ifdef USE_INSTANCING_MORPH
	float morphTargetInfluences[ MORPHTARGETS_COUNT ];
	float morphTargetBaseInfluence = texelFetch( morphTexture, ivec2( 0, gl_InstanceID ), 0 ).r;
	for ( int i = 0; i < MORPHTARGETS_COUNT; i ++ ) {
		morphTargetInfluences[i] =  texelFetch( morphTexture, ivec2( i + 1, gl_InstanceID ), 0 ).r;
	}
#endif`,um=`#if defined( USE_MORPHCOLORS )
	vColor *= morphTargetBaseInfluence;
	for ( int i = 0; i < MORPHTARGETS_COUNT; i ++ ) {
		#if defined( USE_COLOR_ALPHA )
			if ( morphTargetInfluences[ i ] != 0.0 ) vColor += getMorph( gl_VertexID, i, 2 ) * morphTargetInfluences[ i ];
		#elif defined( USE_COLOR )
			if ( morphTargetInfluences[ i ] != 0.0 ) vColor += getMorph( gl_VertexID, i, 2 ).rgb * morphTargetInfluences[ i ];
		#endif
	}
#endif`,dm=`#ifdef USE_MORPHNORMALS
	objectNormal *= morphTargetBaseInfluence;
	for ( int i = 0; i < MORPHTARGETS_COUNT; i ++ ) {
		if ( morphTargetInfluences[ i ] != 0.0 ) objectNormal += getMorph( gl_VertexID, i, 1 ).xyz * morphTargetInfluences[ i ];
	}
#endif`,hm=`#ifdef USE_MORPHTARGETS
	#ifndef USE_INSTANCING_MORPH
		uniform float morphTargetBaseInfluence;
		uniform float morphTargetInfluences[ MORPHTARGETS_COUNT ];
	#endif
	uniform sampler2DArray morphTargetsTexture;
	uniform ivec2 morphTargetsTextureSize;
	vec4 getMorph( const in int vertexIndex, const in int morphTargetIndex, const in int offset ) {
		int texelIndex = vertexIndex * MORPHTARGETS_TEXTURE_STRIDE + offset;
		int y = texelIndex / morphTargetsTextureSize.x;
		int x = texelIndex - y * morphTargetsTextureSize.x;
		ivec3 morphUV = ivec3( x, y, morphTargetIndex );
		return texelFetch( morphTargetsTexture, morphUV, 0 );
	}
#endif`,fm=`#ifdef USE_MORPHTARGETS
	transformed *= morphTargetBaseInfluence;
	for ( int i = 0; i < MORPHTARGETS_COUNT; i ++ ) {
		if ( morphTargetInfluences[ i ] != 0.0 ) transformed += getMorph( gl_VertexID, i, 0 ).xyz * morphTargetInfluences[ i ];
	}
#endif`,pm=`float faceDirection = gl_FrontFacing ? 1.0 : - 1.0;
#ifdef FLAT_SHADED
	vec3 fdx = dFdx( vViewPosition );
	vec3 fdy = dFdy( vViewPosition );
	vec3 normal = normalize( cross( fdx, fdy ) );
#else
	vec3 normal = normalize( vNormal );
	#ifdef DOUBLE_SIDED
		normal *= faceDirection;
	#endif
#endif
#if defined( USE_NORMALMAP_TANGENTSPACE ) || defined( USE_CLEARCOAT_NORMALMAP ) || defined( USE_ANISOTROPY )
	#ifdef USE_TANGENT
		mat3 tbn = mat3( normalize( vTangent ), normalize( vBitangent ), normal );
	#else
		mat3 tbn = getTangentFrame( - vViewPosition, normal,
		#if defined( USE_NORMALMAP )
			vNormalMapUv
		#elif defined( USE_CLEARCOAT_NORMALMAP )
			vClearcoatNormalMapUv
		#else
			vUv
		#endif
		);
	#endif
	#if defined( DOUBLE_SIDED ) && ! defined( FLAT_SHADED )
		tbn[0] *= faceDirection;
		tbn[1] *= faceDirection;
	#endif
#endif
#ifdef USE_CLEARCOAT_NORMALMAP
	#ifdef USE_TANGENT
		mat3 tbn2 = mat3( normalize( vTangent ), normalize( vBitangent ), normal );
	#else
		mat3 tbn2 = getTangentFrame( - vViewPosition, normal, vClearcoatNormalMapUv );
	#endif
	#if defined( DOUBLE_SIDED ) && ! defined( FLAT_SHADED )
		tbn2[0] *= faceDirection;
		tbn2[1] *= faceDirection;
	#endif
#endif
vec3 nonPerturbedNormal = normal;`,mm=`#ifdef USE_NORMALMAP_OBJECTSPACE
	normal = texture2D( normalMap, vNormalMapUv ).xyz * 2.0 - 1.0;
	#ifdef FLIP_SIDED
		normal = - normal;
	#endif
	#ifdef DOUBLE_SIDED
		normal = normal * faceDirection;
	#endif
	normal = normalize( normalMatrix * normal );
#elif defined( USE_NORMALMAP_TANGENTSPACE )
	vec3 mapN = texture2D( normalMap, vNormalMapUv ).xyz * 2.0 - 1.0;
	mapN.xy *= normalScale;
	normal = normalize( tbn * mapN );
#elif defined( USE_BUMPMAP )
	normal = perturbNormalArb( - vViewPosition, normal, dHdxy_fwd(), faceDirection );
#endif`,gm=`#ifndef FLAT_SHADED
	varying vec3 vNormal;
	#ifdef USE_TANGENT
		varying vec3 vTangent;
		varying vec3 vBitangent;
	#endif
#endif`,_m=`#ifndef FLAT_SHADED
	varying vec3 vNormal;
	#ifdef USE_TANGENT
		varying vec3 vTangent;
		varying vec3 vBitangent;
	#endif
#endif`,vm=`#ifndef FLAT_SHADED
	vNormal = normalize( transformedNormal );
	#ifdef USE_TANGENT
		vTangent = normalize( transformedTangent );
		vBitangent = normalize( cross( vNormal, vTangent ) * tangent.w );
	#endif
#endif`,xm=`#ifdef USE_NORMALMAP
	uniform sampler2D normalMap;
	uniform vec2 normalScale;
#endif
#ifdef USE_NORMALMAP_OBJECTSPACE
	uniform mat3 normalMatrix;
#endif
#if ! defined ( USE_TANGENT ) && ( defined ( USE_NORMALMAP_TANGENTSPACE ) || defined ( USE_CLEARCOAT_NORMALMAP ) || defined( USE_ANISOTROPY ) )
	mat3 getTangentFrame( vec3 eye_pos, vec3 surf_norm, vec2 uv ) {
		vec3 q0 = dFdx( eye_pos.xyz );
		vec3 q1 = dFdy( eye_pos.xyz );
		vec2 st0 = dFdx( uv.st );
		vec2 st1 = dFdy( uv.st );
		vec3 N = surf_norm;
		vec3 q1perp = cross( q1, N );
		vec3 q0perp = cross( N, q0 );
		vec3 T = q1perp * st0.x + q0perp * st1.x;
		vec3 B = q1perp * st0.y + q0perp * st1.y;
		float det = max( dot( T, T ), dot( B, B ) );
		float scale = ( det == 0.0 ) ? 0.0 : inversesqrt( det );
		return mat3( T * scale, B * scale, N );
	}
#endif`,Mm=`#ifdef USE_CLEARCOAT
	vec3 clearcoatNormal = nonPerturbedNormal;
#endif`,Sm=`#ifdef USE_CLEARCOAT_NORMALMAP
	vec3 clearcoatMapN = texture2D( clearcoatNormalMap, vClearcoatNormalMapUv ).xyz * 2.0 - 1.0;
	clearcoatMapN.xy *= clearcoatNormalScale;
	clearcoatNormal = normalize( tbn2 * clearcoatMapN );
#endif`,ym=`#ifdef USE_CLEARCOATMAP
	uniform sampler2D clearcoatMap;
#endif
#ifdef USE_CLEARCOAT_NORMALMAP
	uniform sampler2D clearcoatNormalMap;
	uniform vec2 clearcoatNormalScale;
#endif
#ifdef USE_CLEARCOAT_ROUGHNESSMAP
	uniform sampler2D clearcoatRoughnessMap;
#endif`,bm=`#ifdef USE_IRIDESCENCEMAP
	uniform sampler2D iridescenceMap;
#endif
#ifdef USE_IRIDESCENCE_THICKNESSMAP
	uniform sampler2D iridescenceThicknessMap;
#endif`,Em=`#ifdef OPAQUE
diffuseColor.a = 1.0;
#endif
#ifdef USE_TRANSMISSION
diffuseColor.a *= material.transmissionAlpha;
#endif
gl_FragColor = vec4( outgoingLight, diffuseColor.a );`,Tm=`vec3 packNormalToRGB( const in vec3 normal ) {
	return normalize( normal ) * 0.5 + 0.5;
}
vec3 unpackRGBToNormal( const in vec3 rgb ) {
	return 2.0 * rgb.xyz - 1.0;
}
const float PackUpscale = 256. / 255.;const float UnpackDownscale = 255. / 256.;const float ShiftRight8 = 1. / 256.;
const float Inv255 = 1. / 255.;
const vec4 PackFactors = vec4( 1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0 );
const vec2 UnpackFactors2 = vec2( UnpackDownscale, 1.0 / PackFactors.g );
const vec3 UnpackFactors3 = vec3( UnpackDownscale / PackFactors.rg, 1.0 / PackFactors.b );
const vec4 UnpackFactors4 = vec4( UnpackDownscale / PackFactors.rgb, 1.0 / PackFactors.a );
vec4 packDepthToRGBA( const in float v ) {
	if( v <= 0.0 )
		return vec4( 0., 0., 0., 0. );
	if( v >= 1.0 )
		return vec4( 1., 1., 1., 1. );
	float vuf;
	float af = modf( v * PackFactors.a, vuf );
	float bf = modf( vuf * ShiftRight8, vuf );
	float gf = modf( vuf * ShiftRight8, vuf );
	return vec4( vuf * Inv255, gf * PackUpscale, bf * PackUpscale, af );
}
vec3 packDepthToRGB( const in float v ) {
	if( v <= 0.0 )
		return vec3( 0., 0., 0. );
	if( v >= 1.0 )
		return vec3( 1., 1., 1. );
	float vuf;
	float bf = modf( v * PackFactors.b, vuf );
	float gf = modf( vuf * ShiftRight8, vuf );
	return vec3( vuf * Inv255, gf * PackUpscale, bf );
}
vec2 packDepthToRG( const in float v ) {
	if( v <= 0.0 )
		return vec2( 0., 0. );
	if( v >= 1.0 )
		return vec2( 1., 1. );
	float vuf;
	float gf = modf( v * 256., vuf );
	return vec2( vuf * Inv255, gf );
}
float unpackRGBAToDepth( const in vec4 v ) {
	return dot( v, UnpackFactors4 );
}
float unpackRGBToDepth( const in vec3 v ) {
	return dot( v, UnpackFactors3 );
}
float unpackRGToDepth( const in vec2 v ) {
	return v.r * UnpackFactors2.r + v.g * UnpackFactors2.g;
}
vec4 pack2HalfToRGBA( const in vec2 v ) {
	vec4 r = vec4( v.x, fract( v.x * 255.0 ), v.y, fract( v.y * 255.0 ) );
	return vec4( r.x - r.y / 255.0, r.y, r.z - r.w / 255.0, r.w );
}
vec2 unpackRGBATo2Half( const in vec4 v ) {
	return vec2( v.x + ( v.y / 255.0 ), v.z + ( v.w / 255.0 ) );
}
float viewZToOrthographicDepth( const in float viewZ, const in float near, const in float far ) {
	return ( viewZ + near ) / ( near - far );
}
float orthographicDepthToViewZ( const in float depth, const in float near, const in float far ) {
	return depth * ( near - far ) - near;
}
float viewZToPerspectiveDepth( const in float viewZ, const in float near, const in float far ) {
	return ( ( near + viewZ ) * far ) / ( ( far - near ) * viewZ );
}
float perspectiveDepthToViewZ( const in float depth, const in float near, const in float far ) {
	return ( near * far ) / ( ( far - near ) * depth - far );
}`,wm=`#ifdef PREMULTIPLIED_ALPHA
	gl_FragColor.rgb *= gl_FragColor.a;
#endif`,Am=`vec4 mvPosition = vec4( transformed, 1.0 );
#ifdef USE_BATCHING
	mvPosition = batchingMatrix * mvPosition;
#endif
#ifdef USE_INSTANCING
	mvPosition = instanceMatrix * mvPosition;
#endif
mvPosition = modelViewMatrix * mvPosition;
gl_Position = projectionMatrix * mvPosition;`,Rm=`#ifdef DITHERING
	gl_FragColor.rgb = dithering( gl_FragColor.rgb );
#endif`,Cm=`#ifdef DITHERING
	vec3 dithering( vec3 color ) {
		float grid_position = rand( gl_FragCoord.xy );
		vec3 dither_shift_RGB = vec3( 0.25 / 255.0, -0.25 / 255.0, 0.25 / 255.0 );
		dither_shift_RGB = mix( 2.0 * dither_shift_RGB, -2.0 * dither_shift_RGB, grid_position );
		return color + dither_shift_RGB;
	}
#endif`,Pm=`float roughnessFactor = roughness;
#ifdef USE_ROUGHNESSMAP
	vec4 texelRoughness = texture2D( roughnessMap, vRoughnessMapUv );
	roughnessFactor *= texelRoughness.g;
#endif`,Lm=`#ifdef USE_ROUGHNESSMAP
	uniform sampler2D roughnessMap;
#endif`,Dm=`#if NUM_SPOT_LIGHT_COORDS > 0
	varying vec4 vSpotLightCoord[ NUM_SPOT_LIGHT_COORDS ];
#endif
#if NUM_SPOT_LIGHT_MAPS > 0
	uniform sampler2D spotLightMap[ NUM_SPOT_LIGHT_MAPS ];
#endif
#ifdef USE_SHADOWMAP
	#if NUM_DIR_LIGHT_SHADOWS > 0
		uniform sampler2D directionalShadowMap[ NUM_DIR_LIGHT_SHADOWS ];
		varying vec4 vDirectionalShadowCoord[ NUM_DIR_LIGHT_SHADOWS ];
		struct DirectionalLightShadow {
			float shadowIntensity;
			float shadowBias;
			float shadowNormalBias;
			float shadowRadius;
			vec2 shadowMapSize;
		};
		uniform DirectionalLightShadow directionalLightShadows[ NUM_DIR_LIGHT_SHADOWS ];
	#endif
	#if NUM_SPOT_LIGHT_SHADOWS > 0
		uniform sampler2D spotShadowMap[ NUM_SPOT_LIGHT_SHADOWS ];
		struct SpotLightShadow {
			float shadowIntensity;
			float shadowBias;
			float shadowNormalBias;
			float shadowRadius;
			vec2 shadowMapSize;
		};
		uniform SpotLightShadow spotLightShadows[ NUM_SPOT_LIGHT_SHADOWS ];
	#endif
	#if NUM_POINT_LIGHT_SHADOWS > 0
		uniform sampler2D pointShadowMap[ NUM_POINT_LIGHT_SHADOWS ];
		varying vec4 vPointShadowCoord[ NUM_POINT_LIGHT_SHADOWS ];
		struct PointLightShadow {
			float shadowIntensity;
			float shadowBias;
			float shadowNormalBias;
			float shadowRadius;
			vec2 shadowMapSize;
			float shadowCameraNear;
			float shadowCameraFar;
		};
		uniform PointLightShadow pointLightShadows[ NUM_POINT_LIGHT_SHADOWS ];
	#endif
	float texture2DCompare( sampler2D depths, vec2 uv, float compare ) {
		float depth = unpackRGBAToDepth( texture2D( depths, uv ) );
		#ifdef USE_REVERSEDEPTHBUF
			return step( depth, compare );
		#else
			return step( compare, depth );
		#endif
	}
	vec2 texture2DDistribution( sampler2D shadow, vec2 uv ) {
		return unpackRGBATo2Half( texture2D( shadow, uv ) );
	}
	float VSMShadow (sampler2D shadow, vec2 uv, float compare ){
		float occlusion = 1.0;
		vec2 distribution = texture2DDistribution( shadow, uv );
		#ifdef USE_REVERSEDEPTHBUF
			float hard_shadow = step( distribution.x, compare );
		#else
			float hard_shadow = step( compare , distribution.x );
		#endif
		if (hard_shadow != 1.0 ) {
			float distance = compare - distribution.x ;
			float variance = max( 0.00000, distribution.y * distribution.y );
			float softness_probability = variance / (variance + distance * distance );			softness_probability = clamp( ( softness_probability - 0.3 ) / ( 0.95 - 0.3 ), 0.0, 1.0 );			occlusion = clamp( max( hard_shadow, softness_probability ), 0.0, 1.0 );
		}
		return occlusion;
	}
	float getShadow( sampler2D shadowMap, vec2 shadowMapSize, float shadowIntensity, float shadowBias, float shadowRadius, vec4 shadowCoord ) {
		float shadow = 1.0;
		shadowCoord.xyz /= shadowCoord.w;
		shadowCoord.z += shadowBias;
		bool inFrustum = shadowCoord.x >= 0.0 && shadowCoord.x <= 1.0 && shadowCoord.y >= 0.0 && shadowCoord.y <= 1.0;
		bool frustumTest = inFrustum && shadowCoord.z <= 1.0;
		if ( frustumTest ) {
		#if defined( SHADOWMAP_TYPE_PCF )
			vec2 texelSize = vec2( 1.0 ) / shadowMapSize;
			float dx0 = - texelSize.x * shadowRadius;
			float dy0 = - texelSize.y * shadowRadius;
			float dx1 = + texelSize.x * shadowRadius;
			float dy1 = + texelSize.y * shadowRadius;
			float dx2 = dx0 / 2.0;
			float dy2 = dy0 / 2.0;
			float dx3 = dx1 / 2.0;
			float dy3 = dy1 / 2.0;
			shadow = (
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx0, dy0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( 0.0, dy0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx1, dy0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx2, dy2 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( 0.0, dy2 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx3, dy2 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx0, 0.0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx2, 0.0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy, shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx3, 0.0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx1, 0.0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx2, dy3 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( 0.0, dy3 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx3, dy3 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx0, dy1 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( 0.0, dy1 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, shadowCoord.xy + vec2( dx1, dy1 ), shadowCoord.z )
			) * ( 1.0 / 17.0 );
		#elif defined( SHADOWMAP_TYPE_PCF_SOFT )
			vec2 texelSize = vec2( 1.0 ) / shadowMapSize;
			float dx = texelSize.x;
			float dy = texelSize.y;
			vec2 uv = shadowCoord.xy;
			vec2 f = fract( uv * shadowMapSize + 0.5 );
			uv -= f * texelSize;
			shadow = (
				texture2DCompare( shadowMap, uv, shadowCoord.z ) +
				texture2DCompare( shadowMap, uv + vec2( dx, 0.0 ), shadowCoord.z ) +
				texture2DCompare( shadowMap, uv + vec2( 0.0, dy ), shadowCoord.z ) +
				texture2DCompare( shadowMap, uv + texelSize, shadowCoord.z ) +
				mix( texture2DCompare( shadowMap, uv + vec2( -dx, 0.0 ), shadowCoord.z ),
					 texture2DCompare( shadowMap, uv + vec2( 2.0 * dx, 0.0 ), shadowCoord.z ),
					 f.x ) +
				mix( texture2DCompare( shadowMap, uv + vec2( -dx, dy ), shadowCoord.z ),
					 texture2DCompare( shadowMap, uv + vec2( 2.0 * dx, dy ), shadowCoord.z ),
					 f.x ) +
				mix( texture2DCompare( shadowMap, uv + vec2( 0.0, -dy ), shadowCoord.z ),
					 texture2DCompare( shadowMap, uv + vec2( 0.0, 2.0 * dy ), shadowCoord.z ),
					 f.y ) +
				mix( texture2DCompare( shadowMap, uv + vec2( dx, -dy ), shadowCoord.z ),
					 texture2DCompare( shadowMap, uv + vec2( dx, 2.0 * dy ), shadowCoord.z ),
					 f.y ) +
				mix( mix( texture2DCompare( shadowMap, uv + vec2( -dx, -dy ), shadowCoord.z ),
						  texture2DCompare( shadowMap, uv + vec2( 2.0 * dx, -dy ), shadowCoord.z ),
						  f.x ),
					 mix( texture2DCompare( shadowMap, uv + vec2( -dx, 2.0 * dy ), shadowCoord.z ),
						  texture2DCompare( shadowMap, uv + vec2( 2.0 * dx, 2.0 * dy ), shadowCoord.z ),
						  f.x ),
					 f.y )
			) * ( 1.0 / 9.0 );
		#elif defined( SHADOWMAP_TYPE_VSM )
			shadow = VSMShadow( shadowMap, shadowCoord.xy, shadowCoord.z );
		#else
			shadow = texture2DCompare( shadowMap, shadowCoord.xy, shadowCoord.z );
		#endif
		}
		return mix( 1.0, shadow, shadowIntensity );
	}
	vec2 cubeToUV( vec3 v, float texelSizeY ) {
		vec3 absV = abs( v );
		float scaleToCube = 1.0 / max( absV.x, max( absV.y, absV.z ) );
		absV *= scaleToCube;
		v *= scaleToCube * ( 1.0 - 2.0 * texelSizeY );
		vec2 planar = v.xy;
		float almostATexel = 1.5 * texelSizeY;
		float almostOne = 1.0 - almostATexel;
		if ( absV.z >= almostOne ) {
			if ( v.z > 0.0 )
				planar.x = 4.0 - v.x;
		} else if ( absV.x >= almostOne ) {
			float signX = sign( v.x );
			planar.x = v.z * signX + 2.0 * signX;
		} else if ( absV.y >= almostOne ) {
			float signY = sign( v.y );
			planar.x = v.x + 2.0 * signY + 2.0;
			planar.y = v.z * signY - 2.0;
		}
		return vec2( 0.125, 0.25 ) * planar + vec2( 0.375, 0.75 );
	}
	float getPointShadow( sampler2D shadowMap, vec2 shadowMapSize, float shadowIntensity, float shadowBias, float shadowRadius, vec4 shadowCoord, float shadowCameraNear, float shadowCameraFar ) {
		float shadow = 1.0;
		vec3 lightToPosition = shadowCoord.xyz;
		
		float lightToPositionLength = length( lightToPosition );
		if ( lightToPositionLength - shadowCameraFar <= 0.0 && lightToPositionLength - shadowCameraNear >= 0.0 ) {
			float dp = ( lightToPositionLength - shadowCameraNear ) / ( shadowCameraFar - shadowCameraNear );			dp += shadowBias;
			vec3 bd3D = normalize( lightToPosition );
			vec2 texelSize = vec2( 1.0 ) / ( shadowMapSize * vec2( 4.0, 2.0 ) );
			#if defined( SHADOWMAP_TYPE_PCF ) || defined( SHADOWMAP_TYPE_PCF_SOFT ) || defined( SHADOWMAP_TYPE_VSM )
				vec2 offset = vec2( - 1, 1 ) * shadowRadius * texelSize.y;
				shadow = (
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xyy, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yyy, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xyx, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yyx, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xxy, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yxy, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.xxx, texelSize.y ), dp ) +
					texture2DCompare( shadowMap, cubeToUV( bd3D + offset.yxx, texelSize.y ), dp )
				) * ( 1.0 / 9.0 );
			#else
				shadow = texture2DCompare( shadowMap, cubeToUV( bd3D, texelSize.y ), dp );
			#endif
		}
		return mix( 1.0, shadow, shadowIntensity );
	}
#endif`,Im=`#if NUM_SPOT_LIGHT_COORDS > 0
	uniform mat4 spotLightMatrix[ NUM_SPOT_LIGHT_COORDS ];
	varying vec4 vSpotLightCoord[ NUM_SPOT_LIGHT_COORDS ];
#endif
#ifdef USE_SHADOWMAP
	#if NUM_DIR_LIGHT_SHADOWS > 0
		uniform mat4 directionalShadowMatrix[ NUM_DIR_LIGHT_SHADOWS ];
		varying vec4 vDirectionalShadowCoord[ NUM_DIR_LIGHT_SHADOWS ];
		struct DirectionalLightShadow {
			float shadowIntensity;
			float shadowBias;
			float shadowNormalBias;
			float shadowRadius;
			vec2 shadowMapSize;
		};
		uniform DirectionalLightShadow directionalLightShadows[ NUM_DIR_LIGHT_SHADOWS ];
	#endif
	#if NUM_SPOT_LIGHT_SHADOWS > 0
		struct SpotLightShadow {
			float shadowIntensity;
			float shadowBias;
			float shadowNormalBias;
			float shadowRadius;
			vec2 shadowMapSize;
		};
		uniform SpotLightShadow spotLightShadows[ NUM_SPOT_LIGHT_SHADOWS ];
	#endif
	#if NUM_POINT_LIGHT_SHADOWS > 0
		uniform mat4 pointShadowMatrix[ NUM_POINT_LIGHT_SHADOWS ];
		varying vec4 vPointShadowCoord[ NUM_POINT_LIGHT_SHADOWS ];
		struct PointLightShadow {
			float shadowIntensity;
			float shadowBias;
			float shadowNormalBias;
			float shadowRadius;
			vec2 shadowMapSize;
			float shadowCameraNear;
			float shadowCameraFar;
		};
		uniform PointLightShadow pointLightShadows[ NUM_POINT_LIGHT_SHADOWS ];
	#endif
#endif`,Um=`#if ( defined( USE_SHADOWMAP ) && ( NUM_DIR_LIGHT_SHADOWS > 0 || NUM_POINT_LIGHT_SHADOWS > 0 ) ) || ( NUM_SPOT_LIGHT_COORDS > 0 )
	vec3 shadowWorldNormal = inverseTransformDirection( transformedNormal, viewMatrix );
	vec4 shadowWorldPosition;
#endif
#if defined( USE_SHADOWMAP )
	#if NUM_DIR_LIGHT_SHADOWS > 0
		#pragma unroll_loop_start
		for ( int i = 0; i < NUM_DIR_LIGHT_SHADOWS; i ++ ) {
			shadowWorldPosition = worldPosition + vec4( shadowWorldNormal * directionalLightShadows[ i ].shadowNormalBias, 0 );
			vDirectionalShadowCoord[ i ] = directionalShadowMatrix[ i ] * shadowWorldPosition;
		}
		#pragma unroll_loop_end
	#endif
	#if NUM_POINT_LIGHT_SHADOWS > 0
		#pragma unroll_loop_start
		for ( int i = 0; i < NUM_POINT_LIGHT_SHADOWS; i ++ ) {
			shadowWorldPosition = worldPosition + vec4( shadowWorldNormal * pointLightShadows[ i ].shadowNormalBias, 0 );
			vPointShadowCoord[ i ] = pointShadowMatrix[ i ] * shadowWorldPosition;
		}
		#pragma unroll_loop_end
	#endif
#endif
#if NUM_SPOT_LIGHT_COORDS > 0
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_SPOT_LIGHT_COORDS; i ++ ) {
		shadowWorldPosition = worldPosition;
		#if ( defined( USE_SHADOWMAP ) && UNROLLED_LOOP_INDEX < NUM_SPOT_LIGHT_SHADOWS )
			shadowWorldPosition.xyz += shadowWorldNormal * spotLightShadows[ i ].shadowNormalBias;
		#endif
		vSpotLightCoord[ i ] = spotLightMatrix[ i ] * shadowWorldPosition;
	}
	#pragma unroll_loop_end
#endif`,km=`float getShadowMask() {
	float shadow = 1.0;
	#ifdef USE_SHADOWMAP
	#if NUM_DIR_LIGHT_SHADOWS > 0
	DirectionalLightShadow directionalLight;
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_DIR_LIGHT_SHADOWS; i ++ ) {
		directionalLight = directionalLightShadows[ i ];
		shadow *= receiveShadow ? getShadow( directionalShadowMap[ i ], directionalLight.shadowMapSize, directionalLight.shadowIntensity, directionalLight.shadowBias, directionalLight.shadowRadius, vDirectionalShadowCoord[ i ] ) : 1.0;
	}
	#pragma unroll_loop_end
	#endif
	#if NUM_SPOT_LIGHT_SHADOWS > 0
	SpotLightShadow spotLight;
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_SPOT_LIGHT_SHADOWS; i ++ ) {
		spotLight = spotLightShadows[ i ];
		shadow *= receiveShadow ? getShadow( spotShadowMap[ i ], spotLight.shadowMapSize, spotLight.shadowIntensity, spotLight.shadowBias, spotLight.shadowRadius, vSpotLightCoord[ i ] ) : 1.0;
	}
	#pragma unroll_loop_end
	#endif
	#if NUM_POINT_LIGHT_SHADOWS > 0
	PointLightShadow pointLight;
	#pragma unroll_loop_start
	for ( int i = 0; i < NUM_POINT_LIGHT_SHADOWS; i ++ ) {
		pointLight = pointLightShadows[ i ];
		shadow *= receiveShadow ? getPointShadow( pointShadowMap[ i ], pointLight.shadowMapSize, pointLight.shadowIntensity, pointLight.shadowBias, pointLight.shadowRadius, vPointShadowCoord[ i ], pointLight.shadowCameraNear, pointLight.shadowCameraFar ) : 1.0;
	}
	#pragma unroll_loop_end
	#endif
	#endif
	return shadow;
}`,Nm=`#ifdef USE_SKINNING
	mat4 boneMatX = getBoneMatrix( skinIndex.x );
	mat4 boneMatY = getBoneMatrix( skinIndex.y );
	mat4 boneMatZ = getBoneMatrix( skinIndex.z );
	mat4 boneMatW = getBoneMatrix( skinIndex.w );
#endif`,Fm=`#ifdef USE_SKINNING
	uniform mat4 bindMatrix;
	uniform mat4 bindMatrixInverse;
	uniform highp sampler2D boneTexture;
	mat4 getBoneMatrix( const in float i ) {
		int size = textureSize( boneTexture, 0 ).x;
		int j = int( i ) * 4;
		int x = j % size;
		int y = j / size;
		vec4 v1 = texelFetch( boneTexture, ivec2( x, y ), 0 );
		vec4 v2 = texelFetch( boneTexture, ivec2( x + 1, y ), 0 );
		vec4 v3 = texelFetch( boneTexture, ivec2( x + 2, y ), 0 );
		vec4 v4 = texelFetch( boneTexture, ivec2( x + 3, y ), 0 );
		return mat4( v1, v2, v3, v4 );
	}
#endif`,Om=`#ifdef USE_SKINNING
	vec4 skinVertex = bindMatrix * vec4( transformed, 1.0 );
	vec4 skinned = vec4( 0.0 );
	skinned += boneMatX * skinVertex * skinWeight.x;
	skinned += boneMatY * skinVertex * skinWeight.y;
	skinned += boneMatZ * skinVertex * skinWeight.z;
	skinned += boneMatW * skinVertex * skinWeight.w;
	transformed = ( bindMatrixInverse * skinned ).xyz;
#endif`,Bm=`#ifdef USE_SKINNING
	mat4 skinMatrix = mat4( 0.0 );
	skinMatrix += skinWeight.x * boneMatX;
	skinMatrix += skinWeight.y * boneMatY;
	skinMatrix += skinWeight.z * boneMatZ;
	skinMatrix += skinWeight.w * boneMatW;
	skinMatrix = bindMatrixInverse * skinMatrix * bindMatrix;
	objectNormal = vec4( skinMatrix * vec4( objectNormal, 0.0 ) ).xyz;
	#ifdef USE_TANGENT
		objectTangent = vec4( skinMatrix * vec4( objectTangent, 0.0 ) ).xyz;
	#endif
#endif`,zm=`float specularStrength;
#ifdef USE_SPECULARMAP
	vec4 texelSpecular = texture2D( specularMap, vSpecularMapUv );
	specularStrength = texelSpecular.r;
#else
	specularStrength = 1.0;
#endif`,Vm=`#ifdef USE_SPECULARMAP
	uniform sampler2D specularMap;
#endif`,Hm=`#if defined( TONE_MAPPING )
	gl_FragColor.rgb = toneMapping( gl_FragColor.rgb );
#endif`,Gm=`#ifndef saturate
#define saturate( a ) clamp( a, 0.0, 1.0 )
#endif
uniform float toneMappingExposure;
vec3 LinearToneMapping( vec3 color ) {
	return saturate( toneMappingExposure * color );
}
vec3 ReinhardToneMapping( vec3 color ) {
	color *= toneMappingExposure;
	return saturate( color / ( vec3( 1.0 ) + color ) );
}
vec3 CineonToneMapping( vec3 color ) {
	color *= toneMappingExposure;
	color = max( vec3( 0.0 ), color - 0.004 );
	return pow( ( color * ( 6.2 * color + 0.5 ) ) / ( color * ( 6.2 * color + 1.7 ) + 0.06 ), vec3( 2.2 ) );
}
vec3 RRTAndODTFit( vec3 v ) {
	vec3 a = v * ( v + 0.0245786 ) - 0.000090537;
	vec3 b = v * ( 0.983729 * v + 0.4329510 ) + 0.238081;
	return a / b;
}
vec3 ACESFilmicToneMapping( vec3 color ) {
	const mat3 ACESInputMat = mat3(
		vec3( 0.59719, 0.07600, 0.02840 ),		vec3( 0.35458, 0.90834, 0.13383 ),
		vec3( 0.04823, 0.01566, 0.83777 )
	);
	const mat3 ACESOutputMat = mat3(
		vec3(  1.60475, -0.10208, -0.00327 ),		vec3( -0.53108,  1.10813, -0.07276 ),
		vec3( -0.07367, -0.00605,  1.07602 )
	);
	color *= toneMappingExposure / 0.6;
	color = ACESInputMat * color;
	color = RRTAndODTFit( color );
	color = ACESOutputMat * color;
	return saturate( color );
}
const mat3 LINEAR_REC2020_TO_LINEAR_SRGB = mat3(
	vec3( 1.6605, - 0.1246, - 0.0182 ),
	vec3( - 0.5876, 1.1329, - 0.1006 ),
	vec3( - 0.0728, - 0.0083, 1.1187 )
);
const mat3 LINEAR_SRGB_TO_LINEAR_REC2020 = mat3(
	vec3( 0.6274, 0.0691, 0.0164 ),
	vec3( 0.3293, 0.9195, 0.0880 ),
	vec3( 0.0433, 0.0113, 0.8956 )
);
vec3 agxDefaultContrastApprox( vec3 x ) {
	vec3 x2 = x * x;
	vec3 x4 = x2 * x2;
	return + 15.5 * x4 * x2
		- 40.14 * x4 * x
		+ 31.96 * x4
		- 6.868 * x2 * x
		+ 0.4298 * x2
		+ 0.1191 * x
		- 0.00232;
}
vec3 AgXToneMapping( vec3 color ) {
	const mat3 AgXInsetMatrix = mat3(
		vec3( 0.856627153315983, 0.137318972929847, 0.11189821299995 ),
		vec3( 0.0951212405381588, 0.761241990602591, 0.0767994186031903 ),
		vec3( 0.0482516061458583, 0.101439036467562, 0.811302368396859 )
	);
	const mat3 AgXOutsetMatrix = mat3(
		vec3( 1.1271005818144368, - 0.1413297634984383, - 0.14132976349843826 ),
		vec3( - 0.11060664309660323, 1.157823702216272, - 0.11060664309660294 ),
		vec3( - 0.016493938717834573, - 0.016493938717834257, 1.2519364065950405 )
	);
	const float AgxMinEv = - 12.47393;	const float AgxMaxEv = 4.026069;
	color *= toneMappingExposure;
	color = LINEAR_SRGB_TO_LINEAR_REC2020 * color;
	color = AgXInsetMatrix * color;
	color = max( color, 1e-10 );	color = log2( color );
	color = ( color - AgxMinEv ) / ( AgxMaxEv - AgxMinEv );
	color = clamp( color, 0.0, 1.0 );
	color = agxDefaultContrastApprox( color );
	color = AgXOutsetMatrix * color;
	color = pow( max( vec3( 0.0 ), color ), vec3( 2.2 ) );
	color = LINEAR_REC2020_TO_LINEAR_SRGB * color;
	color = clamp( color, 0.0, 1.0 );
	return color;
}
vec3 NeutralToneMapping( vec3 color ) {
	const float StartCompression = 0.8 - 0.04;
	const float Desaturation = 0.15;
	color *= toneMappingExposure;
	float x = min( color.r, min( color.g, color.b ) );
	float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
	color -= offset;
	float peak = max( color.r, max( color.g, color.b ) );
	if ( peak < StartCompression ) return color;
	float d = 1. - StartCompression;
	float newPeak = 1. - d * d / ( peak + d - StartCompression );
	color *= newPeak / peak;
	float g = 1. - 1. / ( Desaturation * ( peak - newPeak ) + 1. );
	return mix( color, vec3( newPeak ), g );
}
vec3 CustomToneMapping( vec3 color ) { return color; }`,Wm=`#ifdef USE_TRANSMISSION
	material.transmission = transmission;
	material.transmissionAlpha = 1.0;
	material.thickness = thickness;
	material.attenuationDistance = attenuationDistance;
	material.attenuationColor = attenuationColor;
	#ifdef USE_TRANSMISSIONMAP
		material.transmission *= texture2D( transmissionMap, vTransmissionMapUv ).r;
	#endif
	#ifdef USE_THICKNESSMAP
		material.thickness *= texture2D( thicknessMap, vThicknessMapUv ).g;
	#endif
	vec3 pos = vWorldPosition;
	vec3 v = normalize( cameraPosition - pos );
	vec3 n = inverseTransformDirection( normal, viewMatrix );
	vec4 transmitted = getIBLVolumeRefraction(
		n, v, material.roughness, material.diffuseColor, material.specularColor, material.specularF90,
		pos, modelMatrix, viewMatrix, projectionMatrix, material.dispersion, material.ior, material.thickness,
		material.attenuationColor, material.attenuationDistance );
	material.transmissionAlpha = mix( material.transmissionAlpha, transmitted.a, material.transmission );
	totalDiffuse = mix( totalDiffuse, transmitted.rgb, material.transmission );
#endif`,$m=`#ifdef USE_TRANSMISSION
	uniform float transmission;
	uniform float thickness;
	uniform float attenuationDistance;
	uniform vec3 attenuationColor;
	#ifdef USE_TRANSMISSIONMAP
		uniform sampler2D transmissionMap;
	#endif
	#ifdef USE_THICKNESSMAP
		uniform sampler2D thicknessMap;
	#endif
	uniform vec2 transmissionSamplerSize;
	uniform sampler2D transmissionSamplerMap;
	uniform mat4 modelMatrix;
	uniform mat4 projectionMatrix;
	varying vec3 vWorldPosition;
	float w0( float a ) {
		return ( 1.0 / 6.0 ) * ( a * ( a * ( - a + 3.0 ) - 3.0 ) + 1.0 );
	}
	float w1( float a ) {
		return ( 1.0 / 6.0 ) * ( a *  a * ( 3.0 * a - 6.0 ) + 4.0 );
	}
	float w2( float a ){
		return ( 1.0 / 6.0 ) * ( a * ( a * ( - 3.0 * a + 3.0 ) + 3.0 ) + 1.0 );
	}
	float w3( float a ) {
		return ( 1.0 / 6.0 ) * ( a * a * a );
	}
	float g0( float a ) {
		return w0( a ) + w1( a );
	}
	float g1( float a ) {
		return w2( a ) + w3( a );
	}
	float h0( float a ) {
		return - 1.0 + w1( a ) / ( w0( a ) + w1( a ) );
	}
	float h1( float a ) {
		return 1.0 + w3( a ) / ( w2( a ) + w3( a ) );
	}
	vec4 bicubic( sampler2D tex, vec2 uv, vec4 texelSize, float lod ) {
		uv = uv * texelSize.zw + 0.5;
		vec2 iuv = floor( uv );
		vec2 fuv = fract( uv );
		float g0x = g0( fuv.x );
		float g1x = g1( fuv.x );
		float h0x = h0( fuv.x );
		float h1x = h1( fuv.x );
		float h0y = h0( fuv.y );
		float h1y = h1( fuv.y );
		vec2 p0 = ( vec2( iuv.x + h0x, iuv.y + h0y ) - 0.5 ) * texelSize.xy;
		vec2 p1 = ( vec2( iuv.x + h1x, iuv.y + h0y ) - 0.5 ) * texelSize.xy;
		vec2 p2 = ( vec2( iuv.x + h0x, iuv.y + h1y ) - 0.5 ) * texelSize.xy;
		vec2 p3 = ( vec2( iuv.x + h1x, iuv.y + h1y ) - 0.5 ) * texelSize.xy;
		return g0( fuv.y ) * ( g0x * textureLod( tex, p0, lod ) + g1x * textureLod( tex, p1, lod ) ) +
			g1( fuv.y ) * ( g0x * textureLod( tex, p2, lod ) + g1x * textureLod( tex, p3, lod ) );
	}
	vec4 textureBicubic( sampler2D sampler, vec2 uv, float lod ) {
		vec2 fLodSize = vec2( textureSize( sampler, int( lod ) ) );
		vec2 cLodSize = vec2( textureSize( sampler, int( lod + 1.0 ) ) );
		vec2 fLodSizeInv = 1.0 / fLodSize;
		vec2 cLodSizeInv = 1.0 / cLodSize;
		vec4 fSample = bicubic( sampler, uv, vec4( fLodSizeInv, fLodSize ), floor( lod ) );
		vec4 cSample = bicubic( sampler, uv, vec4( cLodSizeInv, cLodSize ), ceil( lod ) );
		return mix( fSample, cSample, fract( lod ) );
	}
	vec3 getVolumeTransmissionRay( const in vec3 n, const in vec3 v, const in float thickness, const in float ior, const in mat4 modelMatrix ) {
		vec3 refractionVector = refract( - v, normalize( n ), 1.0 / ior );
		vec3 modelScale;
		modelScale.x = length( vec3( modelMatrix[ 0 ].xyz ) );
		modelScale.y = length( vec3( modelMatrix[ 1 ].xyz ) );
		modelScale.z = length( vec3( modelMatrix[ 2 ].xyz ) );
		return normalize( refractionVector ) * thickness * modelScale;
	}
	float applyIorToRoughness( const in float roughness, const in float ior ) {
		return roughness * clamp( ior * 2.0 - 2.0, 0.0, 1.0 );
	}
	vec4 getTransmissionSample( const in vec2 fragCoord, const in float roughness, const in float ior ) {
		float lod = log2( transmissionSamplerSize.x ) * applyIorToRoughness( roughness, ior );
		return textureBicubic( transmissionSamplerMap, fragCoord.xy, lod );
	}
	vec3 volumeAttenuation( const in float transmissionDistance, const in vec3 attenuationColor, const in float attenuationDistance ) {
		if ( isinf( attenuationDistance ) ) {
			return vec3( 1.0 );
		} else {
			vec3 attenuationCoefficient = -log( attenuationColor ) / attenuationDistance;
			vec3 transmittance = exp( - attenuationCoefficient * transmissionDistance );			return transmittance;
		}
	}
	vec4 getIBLVolumeRefraction( const in vec3 n, const in vec3 v, const in float roughness, const in vec3 diffuseColor,
		const in vec3 specularColor, const in float specularF90, const in vec3 position, const in mat4 modelMatrix,
		const in mat4 viewMatrix, const in mat4 projMatrix, const in float dispersion, const in float ior, const in float thickness,
		const in vec3 attenuationColor, const in float attenuationDistance ) {
		vec4 transmittedLight;
		vec3 transmittance;
		#ifdef USE_DISPERSION
			float halfSpread = ( ior - 1.0 ) * 0.025 * dispersion;
			vec3 iors = vec3( ior - halfSpread, ior, ior + halfSpread );
			for ( int i = 0; i < 3; i ++ ) {
				vec3 transmissionRay = getVolumeTransmissionRay( n, v, thickness, iors[ i ], modelMatrix );
				vec3 refractedRayExit = position + transmissionRay;
				vec4 ndcPos = projMatrix * viewMatrix * vec4( refractedRayExit, 1.0 );
				vec2 refractionCoords = ndcPos.xy / ndcPos.w;
				refractionCoords += 1.0;
				refractionCoords /= 2.0;
				vec4 transmissionSample = getTransmissionSample( refractionCoords, roughness, iors[ i ] );
				transmittedLight[ i ] = transmissionSample[ i ];
				transmittedLight.a += transmissionSample.a;
				transmittance[ i ] = diffuseColor[ i ] * volumeAttenuation( length( transmissionRay ), attenuationColor, attenuationDistance )[ i ];
			}
			transmittedLight.a /= 3.0;
		#else
			vec3 transmissionRay = getVolumeTransmissionRay( n, v, thickness, ior, modelMatrix );
			vec3 refractedRayExit = position + transmissionRay;
			vec4 ndcPos = projMatrix * viewMatrix * vec4( refractedRayExit, 1.0 );
			vec2 refractionCoords = ndcPos.xy / ndcPos.w;
			refractionCoords += 1.0;
			refractionCoords /= 2.0;
			transmittedLight = getTransmissionSample( refractionCoords, roughness, ior );
			transmittance = diffuseColor * volumeAttenuation( length( transmissionRay ), attenuationColor, attenuationDistance );
		#endif
		vec3 attenuatedColor = transmittance * transmittedLight.rgb;
		vec3 F = EnvironmentBRDF( n, v, specularColor, specularF90, roughness );
		float transmittanceFactor = ( transmittance.r + transmittance.g + transmittance.b ) / 3.0;
		return vec4( ( 1.0 - F ) * attenuatedColor, 1.0 - ( 1.0 - transmittedLight.a ) * transmittanceFactor );
	}
#endif`,Xm=`#if defined( USE_UV ) || defined( USE_ANISOTROPY )
	varying vec2 vUv;
#endif
#ifdef USE_MAP
	varying vec2 vMapUv;
#endif
#ifdef USE_ALPHAMAP
	varying vec2 vAlphaMapUv;
#endif
#ifdef USE_LIGHTMAP
	varying vec2 vLightMapUv;
#endif
#ifdef USE_AOMAP
	varying vec2 vAoMapUv;
#endif
#ifdef USE_BUMPMAP
	varying vec2 vBumpMapUv;
#endif
#ifdef USE_NORMALMAP
	varying vec2 vNormalMapUv;
#endif
#ifdef USE_EMISSIVEMAP
	varying vec2 vEmissiveMapUv;
#endif
#ifdef USE_METALNESSMAP
	varying vec2 vMetalnessMapUv;
#endif
#ifdef USE_ROUGHNESSMAP
	varying vec2 vRoughnessMapUv;
#endif
#ifdef USE_ANISOTROPYMAP
	varying vec2 vAnisotropyMapUv;
#endif
#ifdef USE_CLEARCOATMAP
	varying vec2 vClearcoatMapUv;
#endif
#ifdef USE_CLEARCOAT_NORMALMAP
	varying vec2 vClearcoatNormalMapUv;
#endif
#ifdef USE_CLEARCOAT_ROUGHNESSMAP
	varying vec2 vClearcoatRoughnessMapUv;
#endif
#ifdef USE_IRIDESCENCEMAP
	varying vec2 vIridescenceMapUv;
#endif
#ifdef USE_IRIDESCENCE_THICKNESSMAP
	varying vec2 vIridescenceThicknessMapUv;
#endif
#ifdef USE_SHEEN_COLORMAP
	varying vec2 vSheenColorMapUv;
#endif
#ifdef USE_SHEEN_ROUGHNESSMAP
	varying vec2 vSheenRoughnessMapUv;
#endif
#ifdef USE_SPECULARMAP
	varying vec2 vSpecularMapUv;
#endif
#ifdef USE_SPECULAR_COLORMAP
	varying vec2 vSpecularColorMapUv;
#endif
#ifdef USE_SPECULAR_INTENSITYMAP
	varying vec2 vSpecularIntensityMapUv;
#endif
#ifdef USE_TRANSMISSIONMAP
	uniform mat3 transmissionMapTransform;
	varying vec2 vTransmissionMapUv;
#endif
#ifdef USE_THICKNESSMAP
	uniform mat3 thicknessMapTransform;
	varying vec2 vThicknessMapUv;
#endif`,qm=`#if defined( USE_UV ) || defined( USE_ANISOTROPY )
	varying vec2 vUv;
#endif
#ifdef USE_MAP
	uniform mat3 mapTransform;
	varying vec2 vMapUv;
#endif
#ifdef USE_ALPHAMAP
	uniform mat3 alphaMapTransform;
	varying vec2 vAlphaMapUv;
#endif
#ifdef USE_LIGHTMAP
	uniform mat3 lightMapTransform;
	varying vec2 vLightMapUv;
#endif
#ifdef USE_AOMAP
	uniform mat3 aoMapTransform;
	varying vec2 vAoMapUv;
#endif
#ifdef USE_BUMPMAP
	uniform mat3 bumpMapTransform;
	varying vec2 vBumpMapUv;
#endif
#ifdef USE_NORMALMAP
	uniform mat3 normalMapTransform;
	varying vec2 vNormalMapUv;
#endif
#ifdef USE_DISPLACEMENTMAP
	uniform mat3 displacementMapTransform;
	varying vec2 vDisplacementMapUv;
#endif
#ifdef USE_EMISSIVEMAP
	uniform mat3 emissiveMapTransform;
	varying vec2 vEmissiveMapUv;
#endif
#ifdef USE_METALNESSMAP
	uniform mat3 metalnessMapTransform;
	varying vec2 vMetalnessMapUv;
#endif
#ifdef USE_ROUGHNESSMAP
	uniform mat3 roughnessMapTransform;
	varying vec2 vRoughnessMapUv;
#endif
#ifdef USE_ANISOTROPYMAP
	uniform mat3 anisotropyMapTransform;
	varying vec2 vAnisotropyMapUv;
#endif
#ifdef USE_CLEARCOATMAP
	uniform mat3 clearcoatMapTransform;
	varying vec2 vClearcoatMapUv;
#endif
#ifdef USE_CLEARCOAT_NORMALMAP
	uniform mat3 clearcoatNormalMapTransform;
	varying vec2 vClearcoatNormalMapUv;
#endif
#ifdef USE_CLEARCOAT_ROUGHNESSMAP
	uniform mat3 clearcoatRoughnessMapTransform;
	varying vec2 vClearcoatRoughnessMapUv;
#endif
#ifdef USE_SHEEN_COLORMAP
	uniform mat3 sheenColorMapTransform;
	varying vec2 vSheenColorMapUv;
#endif
#ifdef USE_SHEEN_ROUGHNESSMAP
	uniform mat3 sheenRoughnessMapTransform;
	varying vec2 vSheenRoughnessMapUv;
#endif
#ifdef USE_IRIDESCENCEMAP
	uniform mat3 iridescenceMapTransform;
	varying vec2 vIridescenceMapUv;
#endif
#ifdef USE_IRIDESCENCE_THICKNESSMAP
	uniform mat3 iridescenceThicknessMapTransform;
	varying vec2 vIridescenceThicknessMapUv;
#endif
#ifdef USE_SPECULARMAP
	uniform mat3 specularMapTransform;
	varying vec2 vSpecularMapUv;
#endif
#ifdef USE_SPECULAR_COLORMAP
	uniform mat3 specularColorMapTransform;
	varying vec2 vSpecularColorMapUv;
#endif
#ifdef USE_SPECULAR_INTENSITYMAP
	uniform mat3 specularIntensityMapTransform;
	varying vec2 vSpecularIntensityMapUv;
#endif
#ifdef USE_TRANSMISSIONMAP
	uniform mat3 transmissionMapTransform;
	varying vec2 vTransmissionMapUv;
#endif
#ifdef USE_THICKNESSMAP
	uniform mat3 thicknessMapTransform;
	varying vec2 vThicknessMapUv;
#endif`,Ym=`#if defined( USE_UV ) || defined( USE_ANISOTROPY )
	vUv = vec3( uv, 1 ).xy;
#endif
#ifdef USE_MAP
	vMapUv = ( mapTransform * vec3( MAP_UV, 1 ) ).xy;
#endif
#ifdef USE_ALPHAMAP
	vAlphaMapUv = ( alphaMapTransform * vec3( ALPHAMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_LIGHTMAP
	vLightMapUv = ( lightMapTransform * vec3( LIGHTMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_AOMAP
	vAoMapUv = ( aoMapTransform * vec3( AOMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_BUMPMAP
	vBumpMapUv = ( bumpMapTransform * vec3( BUMPMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_NORMALMAP
	vNormalMapUv = ( normalMapTransform * vec3( NORMALMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_DISPLACEMENTMAP
	vDisplacementMapUv = ( displacementMapTransform * vec3( DISPLACEMENTMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_EMISSIVEMAP
	vEmissiveMapUv = ( emissiveMapTransform * vec3( EMISSIVEMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_METALNESSMAP
	vMetalnessMapUv = ( metalnessMapTransform * vec3( METALNESSMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_ROUGHNESSMAP
	vRoughnessMapUv = ( roughnessMapTransform * vec3( ROUGHNESSMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_ANISOTROPYMAP
	vAnisotropyMapUv = ( anisotropyMapTransform * vec3( ANISOTROPYMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_CLEARCOATMAP
	vClearcoatMapUv = ( clearcoatMapTransform * vec3( CLEARCOATMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_CLEARCOAT_NORMALMAP
	vClearcoatNormalMapUv = ( clearcoatNormalMapTransform * vec3( CLEARCOAT_NORMALMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_CLEARCOAT_ROUGHNESSMAP
	vClearcoatRoughnessMapUv = ( clearcoatRoughnessMapTransform * vec3( CLEARCOAT_ROUGHNESSMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_IRIDESCENCEMAP
	vIridescenceMapUv = ( iridescenceMapTransform * vec3( IRIDESCENCEMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_IRIDESCENCE_THICKNESSMAP
	vIridescenceThicknessMapUv = ( iridescenceThicknessMapTransform * vec3( IRIDESCENCE_THICKNESSMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_SHEEN_COLORMAP
	vSheenColorMapUv = ( sheenColorMapTransform * vec3( SHEEN_COLORMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_SHEEN_ROUGHNESSMAP
	vSheenRoughnessMapUv = ( sheenRoughnessMapTransform * vec3( SHEEN_ROUGHNESSMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_SPECULARMAP
	vSpecularMapUv = ( specularMapTransform * vec3( SPECULARMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_SPECULAR_COLORMAP
	vSpecularColorMapUv = ( specularColorMapTransform * vec3( SPECULAR_COLORMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_SPECULAR_INTENSITYMAP
	vSpecularIntensityMapUv = ( specularIntensityMapTransform * vec3( SPECULAR_INTENSITYMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_TRANSMISSIONMAP
	vTransmissionMapUv = ( transmissionMapTransform * vec3( TRANSMISSIONMAP_UV, 1 ) ).xy;
#endif
#ifdef USE_THICKNESSMAP
	vThicknessMapUv = ( thicknessMapTransform * vec3( THICKNESSMAP_UV, 1 ) ).xy;
#endif`,jm=`#if defined( USE_ENVMAP ) || defined( DISTANCE ) || defined ( USE_SHADOWMAP ) || defined ( USE_TRANSMISSION ) || NUM_SPOT_LIGHT_COORDS > 0
	vec4 worldPosition = vec4( transformed, 1.0 );
	#ifdef USE_BATCHING
		worldPosition = batchingMatrix * worldPosition;
	#endif
	#ifdef USE_INSTANCING
		worldPosition = instanceMatrix * worldPosition;
	#endif
	worldPosition = modelMatrix * worldPosition;
#endif`;const Zm=`varying vec2 vUv;
uniform mat3 uvTransform;
void main() {
	vUv = ( uvTransform * vec3( uv, 1 ) ).xy;
	gl_Position = vec4( position.xy, 1.0, 1.0 );
}`,Km=`uniform sampler2D t2D;
uniform float backgroundIntensity;
varying vec2 vUv;
void main() {
	vec4 texColor = texture2D( t2D, vUv );
	#ifdef DECODE_VIDEO_TEXTURE
		texColor = vec4( mix( pow( texColor.rgb * 0.9478672986 + vec3( 0.0521327014 ), vec3( 2.4 ) ), texColor.rgb * 0.0773993808, vec3( lessThanEqual( texColor.rgb, vec3( 0.04045 ) ) ) ), texColor.w );
	#endif
	texColor.rgb *= backgroundIntensity;
	gl_FragColor = texColor;
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
}`,Jm=`varying vec3 vWorldDirection;
#include <common>
void main() {
	vWorldDirection = transformDirection( position, modelMatrix );
	#include <begin_vertex>
	#include <project_vertex>
	gl_Position.z = gl_Position.w;
}`,Qm=`#ifdef ENVMAP_TYPE_CUBE
	uniform samplerCube envMap;
#elif defined( ENVMAP_TYPE_CUBE_UV )
	uniform sampler2D envMap;
#endif
uniform float flipEnvMap;
uniform float backgroundBlurriness;
uniform float backgroundIntensity;
uniform mat3 backgroundRotation;
varying vec3 vWorldDirection;
#include <cube_uv_reflection_fragment>
void main() {
	#ifdef ENVMAP_TYPE_CUBE
		vec4 texColor = textureCube( envMap, backgroundRotation * vec3( flipEnvMap * vWorldDirection.x, vWorldDirection.yz ) );
	#elif defined( ENVMAP_TYPE_CUBE_UV )
		vec4 texColor = textureCubeUV( envMap, backgroundRotation * vWorldDirection, backgroundBlurriness );
	#else
		vec4 texColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	#endif
	texColor.rgb *= backgroundIntensity;
	gl_FragColor = texColor;
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
}`,eg=`varying vec3 vWorldDirection;
#include <common>
void main() {
	vWorldDirection = transformDirection( position, modelMatrix );
	#include <begin_vertex>
	#include <project_vertex>
	gl_Position.z = gl_Position.w;
}`,tg=`uniform samplerCube tCube;
uniform float tFlip;
uniform float opacity;
varying vec3 vWorldDirection;
void main() {
	vec4 texColor = textureCube( tCube, vec3( tFlip * vWorldDirection.x, vWorldDirection.yz ) );
	gl_FragColor = texColor;
	gl_FragColor.a *= opacity;
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
}`,ng=`#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <displacementmap_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
varying vec2 vHighPrecisionZW;
void main() {
	#include <uv_vertex>
	#include <batching_vertex>
	#include <skinbase_vertex>
	#include <morphinstance_vertex>
	#ifdef USE_DISPLACEMENTMAP
		#include <beginnormal_vertex>
		#include <morphnormal_vertex>
		#include <skinnormal_vertex>
	#endif
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	vHighPrecisionZW = gl_Position.zw;
}`,ig=`#if DEPTH_PACKING == 3200
	uniform float opacity;
#endif
#include <common>
#include <packing>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
varying vec2 vHighPrecisionZW;
void main() {
	vec4 diffuseColor = vec4( 1.0 );
	#include <clipping_planes_fragment>
	#if DEPTH_PACKING == 3200
		diffuseColor.a = opacity;
	#endif
	#include <map_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	#include <logdepthbuf_fragment>
	#ifdef USE_REVERSEDEPTHBUF
		float fragCoordZ = vHighPrecisionZW[ 0 ] / vHighPrecisionZW[ 1 ];
	#else
		float fragCoordZ = 0.5 * vHighPrecisionZW[ 0 ] / vHighPrecisionZW[ 1 ] + 0.5;
	#endif
	#if DEPTH_PACKING == 3200
		gl_FragColor = vec4( vec3( 1.0 - fragCoordZ ), opacity );
	#elif DEPTH_PACKING == 3201
		gl_FragColor = packDepthToRGBA( fragCoordZ );
	#elif DEPTH_PACKING == 3202
		gl_FragColor = vec4( packDepthToRGB( fragCoordZ ), 1.0 );
	#elif DEPTH_PACKING == 3203
		gl_FragColor = vec4( packDepthToRG( fragCoordZ ), 0.0, 1.0 );
	#endif
}`,sg=`#define DISTANCE
varying vec3 vWorldPosition;
#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <displacementmap_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <batching_vertex>
	#include <skinbase_vertex>
	#include <morphinstance_vertex>
	#ifdef USE_DISPLACEMENTMAP
		#include <beginnormal_vertex>
		#include <morphnormal_vertex>
		#include <skinnormal_vertex>
	#endif
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <worldpos_vertex>
	#include <clipping_planes_vertex>
	vWorldPosition = worldPosition.xyz;
}`,rg=`#define DISTANCE
uniform vec3 referencePosition;
uniform float nearDistance;
uniform float farDistance;
varying vec3 vWorldPosition;
#include <common>
#include <packing>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <clipping_planes_pars_fragment>
void main () {
	vec4 diffuseColor = vec4( 1.0 );
	#include <clipping_planes_fragment>
	#include <map_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	float dist = length( vWorldPosition - referencePosition );
	dist = ( dist - nearDistance ) / ( farDistance - nearDistance );
	dist = saturate( dist );
	gl_FragColor = packDepthToRGBA( dist );
}`,ag=`varying vec3 vWorldDirection;
#include <common>
void main() {
	vWorldDirection = transformDirection( position, modelMatrix );
	#include <begin_vertex>
	#include <project_vertex>
}`,og=`uniform sampler2D tEquirect;
varying vec3 vWorldDirection;
#include <common>
void main() {
	vec3 direction = normalize( vWorldDirection );
	vec2 sampleUV = equirectUv( direction );
	gl_FragColor = texture2D( tEquirect, sampleUV );
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
}`,lg=`uniform float scale;
attribute float lineDistance;
varying float vLineDistance;
#include <common>
#include <uv_pars_vertex>
#include <color_pars_vertex>
#include <fog_pars_vertex>
#include <morphtarget_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	vLineDistance = scale * lineDistance;
	#include <uv_vertex>
	#include <color_vertex>
	#include <morphinstance_vertex>
	#include <morphcolor_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	#include <fog_vertex>
}`,cg=`uniform vec3 diffuse;
uniform float opacity;
uniform float dashSize;
uniform float totalSize;
varying float vLineDistance;
#include <common>
#include <color_pars_fragment>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <fog_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	if ( mod( vLineDistance, totalSize ) > dashSize ) {
		discard;
	}
	vec3 outgoingLight = vec3( 0.0 );
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <color_fragment>
	outgoingLight = diffuseColor.rgb;
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
}`,ug=`#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <envmap_pars_vertex>
#include <color_pars_vertex>
#include <fog_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <color_vertex>
	#include <morphinstance_vertex>
	#include <morphcolor_vertex>
	#include <batching_vertex>
	#if defined ( USE_ENVMAP ) || defined ( USE_SKINNING )
		#include <beginnormal_vertex>
		#include <morphnormal_vertex>
		#include <skinbase_vertex>
		#include <skinnormal_vertex>
		#include <defaultnormal_vertex>
	#endif
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	#include <worldpos_vertex>
	#include <envmap_vertex>
	#include <fog_vertex>
}`,dg=`uniform vec3 diffuse;
uniform float opacity;
#ifndef FLAT_SHADED
	varying vec3 vNormal;
#endif
#include <common>
#include <dithering_pars_fragment>
#include <color_pars_fragment>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <envmap_common_pars_fragment>
#include <envmap_pars_fragment>
#include <fog_pars_fragment>
#include <specularmap_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	#include <specularmap_fragment>
	ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );
	#ifdef USE_LIGHTMAP
		vec4 lightMapTexel = texture2D( lightMap, vLightMapUv );
		reflectedLight.indirectDiffuse += lightMapTexel.rgb * lightMapIntensity * RECIPROCAL_PI;
	#else
		reflectedLight.indirectDiffuse += vec3( 1.0 );
	#endif
	#include <aomap_fragment>
	reflectedLight.indirectDiffuse *= diffuseColor.rgb;
	vec3 outgoingLight = reflectedLight.indirectDiffuse;
	#include <envmap_fragment>
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
	#include <dithering_fragment>
}`,hg=`#define LAMBERT
varying vec3 vViewPosition;
#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <displacementmap_pars_vertex>
#include <envmap_pars_vertex>
#include <color_pars_vertex>
#include <fog_pars_vertex>
#include <normal_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <shadowmap_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <color_vertex>
	#include <morphinstance_vertex>
	#include <morphcolor_vertex>
	#include <batching_vertex>
	#include <beginnormal_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>
	#include <normal_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	vViewPosition = - mvPosition.xyz;
	#include <worldpos_vertex>
	#include <envmap_vertex>
	#include <shadowmap_vertex>
	#include <fog_vertex>
}`,fg=`#define LAMBERT
uniform vec3 diffuse;
uniform vec3 emissive;
uniform float opacity;
#include <common>
#include <packing>
#include <dithering_pars_fragment>
#include <color_pars_fragment>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <emissivemap_pars_fragment>
#include <envmap_common_pars_fragment>
#include <envmap_pars_fragment>
#include <fog_pars_fragment>
#include <bsdfs>
#include <lights_pars_begin>
#include <normal_pars_fragment>
#include <lights_lambert_pars_fragment>
#include <shadowmap_pars_fragment>
#include <bumpmap_pars_fragment>
#include <normalmap_pars_fragment>
#include <specularmap_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );
	vec3 totalEmissiveRadiance = emissive;
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	#include <specularmap_fragment>
	#include <normal_fragment_begin>
	#include <normal_fragment_maps>
	#include <emissivemap_fragment>
	#include <lights_lambert_fragment>
	#include <lights_fragment_begin>
	#include <lights_fragment_maps>
	#include <lights_fragment_end>
	#include <aomap_fragment>
	vec3 outgoingLight = reflectedLight.directDiffuse + reflectedLight.indirectDiffuse + totalEmissiveRadiance;
	#include <envmap_fragment>
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
	#include <dithering_fragment>
}`,pg=`#define MATCAP
varying vec3 vViewPosition;
#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <color_pars_vertex>
#include <displacementmap_pars_vertex>
#include <fog_pars_vertex>
#include <normal_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <color_vertex>
	#include <morphinstance_vertex>
	#include <morphcolor_vertex>
	#include <batching_vertex>
	#include <beginnormal_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>
	#include <normal_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	#include <fog_vertex>
	vViewPosition = - mvPosition.xyz;
}`,mg=`#define MATCAP
uniform vec3 diffuse;
uniform float opacity;
uniform sampler2D matcap;
varying vec3 vViewPosition;
#include <common>
#include <dithering_pars_fragment>
#include <color_pars_fragment>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <fog_pars_fragment>
#include <normal_pars_fragment>
#include <bumpmap_pars_fragment>
#include <normalmap_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	#include <normal_fragment_begin>
	#include <normal_fragment_maps>
	vec3 viewDir = normalize( vViewPosition );
	vec3 x = normalize( vec3( viewDir.z, 0.0, - viewDir.x ) );
	vec3 y = cross( viewDir, x );
	vec2 uv = vec2( dot( x, normal ), dot( y, normal ) ) * 0.495 + 0.5;
	#ifdef USE_MATCAP
		vec4 matcapColor = texture2D( matcap, uv );
	#else
		vec4 matcapColor = vec4( vec3( mix( 0.2, 0.8, uv.y ) ), 1.0 );
	#endif
	vec3 outgoingLight = diffuseColor.rgb * matcapColor.rgb;
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
	#include <dithering_fragment>
}`,gg=`#define NORMAL
#if defined( FLAT_SHADED ) || defined( USE_BUMPMAP ) || defined( USE_NORMALMAP_TANGENTSPACE )
	varying vec3 vViewPosition;
#endif
#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <displacementmap_pars_vertex>
#include <normal_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <batching_vertex>
	#include <beginnormal_vertex>
	#include <morphinstance_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>
	#include <normal_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
#if defined( FLAT_SHADED ) || defined( USE_BUMPMAP ) || defined( USE_NORMALMAP_TANGENTSPACE )
	vViewPosition = - mvPosition.xyz;
#endif
}`,_g=`#define NORMAL
uniform float opacity;
#if defined( FLAT_SHADED ) || defined( USE_BUMPMAP ) || defined( USE_NORMALMAP_TANGENTSPACE )
	varying vec3 vViewPosition;
#endif
#include <packing>
#include <uv_pars_fragment>
#include <normal_pars_fragment>
#include <bumpmap_pars_fragment>
#include <normalmap_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( 0.0, 0.0, 0.0, opacity );
	#include <clipping_planes_fragment>
	#include <logdepthbuf_fragment>
	#include <normal_fragment_begin>
	#include <normal_fragment_maps>
	gl_FragColor = vec4( packNormalToRGB( normal ), diffuseColor.a );
	#ifdef OPAQUE
		gl_FragColor.a = 1.0;
	#endif
}`,vg=`#define PHONG
varying vec3 vViewPosition;
#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <displacementmap_pars_vertex>
#include <envmap_pars_vertex>
#include <color_pars_vertex>
#include <fog_pars_vertex>
#include <normal_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <shadowmap_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <color_vertex>
	#include <morphcolor_vertex>
	#include <batching_vertex>
	#include <beginnormal_vertex>
	#include <morphinstance_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>
	#include <normal_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	vViewPosition = - mvPosition.xyz;
	#include <worldpos_vertex>
	#include <envmap_vertex>
	#include <shadowmap_vertex>
	#include <fog_vertex>
}`,xg=`#define PHONG
uniform vec3 diffuse;
uniform vec3 emissive;
uniform vec3 specular;
uniform float shininess;
uniform float opacity;
#include <common>
#include <packing>
#include <dithering_pars_fragment>
#include <color_pars_fragment>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <emissivemap_pars_fragment>
#include <envmap_common_pars_fragment>
#include <envmap_pars_fragment>
#include <fog_pars_fragment>
#include <bsdfs>
#include <lights_pars_begin>
#include <normal_pars_fragment>
#include <lights_phong_pars_fragment>
#include <shadowmap_pars_fragment>
#include <bumpmap_pars_fragment>
#include <normalmap_pars_fragment>
#include <specularmap_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );
	vec3 totalEmissiveRadiance = emissive;
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	#include <specularmap_fragment>
	#include <normal_fragment_begin>
	#include <normal_fragment_maps>
	#include <emissivemap_fragment>
	#include <lights_phong_fragment>
	#include <lights_fragment_begin>
	#include <lights_fragment_maps>
	#include <lights_fragment_end>
	#include <aomap_fragment>
	vec3 outgoingLight = reflectedLight.directDiffuse + reflectedLight.indirectDiffuse + reflectedLight.directSpecular + reflectedLight.indirectSpecular + totalEmissiveRadiance;
	#include <envmap_fragment>
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
	#include <dithering_fragment>
}`,Mg=`#define STANDARD
varying vec3 vViewPosition;
#ifdef USE_TRANSMISSION
	varying vec3 vWorldPosition;
#endif
#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <displacementmap_pars_vertex>
#include <color_pars_vertex>
#include <fog_pars_vertex>
#include <normal_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <shadowmap_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <color_vertex>
	#include <morphinstance_vertex>
	#include <morphcolor_vertex>
	#include <batching_vertex>
	#include <beginnormal_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>
	#include <normal_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	vViewPosition = - mvPosition.xyz;
	#include <worldpos_vertex>
	#include <shadowmap_vertex>
	#include <fog_vertex>
#ifdef USE_TRANSMISSION
	vWorldPosition = worldPosition.xyz;
#endif
}`,Sg=`#define STANDARD
#ifdef PHYSICAL
	#define IOR
	#define USE_SPECULAR
#endif
uniform vec3 diffuse;
uniform vec3 emissive;
uniform float roughness;
uniform float metalness;
uniform float opacity;
#ifdef IOR
	uniform float ior;
#endif
#ifdef USE_SPECULAR
	uniform float specularIntensity;
	uniform vec3 specularColor;
	#ifdef USE_SPECULAR_COLORMAP
		uniform sampler2D specularColorMap;
	#endif
	#ifdef USE_SPECULAR_INTENSITYMAP
		uniform sampler2D specularIntensityMap;
	#endif
#endif
#ifdef USE_CLEARCOAT
	uniform float clearcoat;
	uniform float clearcoatRoughness;
#endif
#ifdef USE_DISPERSION
	uniform float dispersion;
#endif
#ifdef USE_IRIDESCENCE
	uniform float iridescence;
	uniform float iridescenceIOR;
	uniform float iridescenceThicknessMinimum;
	uniform float iridescenceThicknessMaximum;
#endif
#ifdef USE_SHEEN
	uniform vec3 sheenColor;
	uniform float sheenRoughness;
	#ifdef USE_SHEEN_COLORMAP
		uniform sampler2D sheenColorMap;
	#endif
	#ifdef USE_SHEEN_ROUGHNESSMAP
		uniform sampler2D sheenRoughnessMap;
	#endif
#endif
#ifdef USE_ANISOTROPY
	uniform vec2 anisotropyVector;
	#ifdef USE_ANISOTROPYMAP
		uniform sampler2D anisotropyMap;
	#endif
#endif
varying vec3 vViewPosition;
#include <common>
#include <packing>
#include <dithering_pars_fragment>
#include <color_pars_fragment>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <emissivemap_pars_fragment>
#include <iridescence_fragment>
#include <cube_uv_reflection_fragment>
#include <envmap_common_pars_fragment>
#include <envmap_physical_pars_fragment>
#include <fog_pars_fragment>
#include <lights_pars_begin>
#include <normal_pars_fragment>
#include <lights_physical_pars_fragment>
#include <transmission_pars_fragment>
#include <shadowmap_pars_fragment>
#include <bumpmap_pars_fragment>
#include <normalmap_pars_fragment>
#include <clearcoat_pars_fragment>
#include <iridescence_pars_fragment>
#include <roughnessmap_pars_fragment>
#include <metalnessmap_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );
	vec3 totalEmissiveRadiance = emissive;
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	#include <roughnessmap_fragment>
	#include <metalnessmap_fragment>
	#include <normal_fragment_begin>
	#include <normal_fragment_maps>
	#include <clearcoat_normal_fragment_begin>
	#include <clearcoat_normal_fragment_maps>
	#include <emissivemap_fragment>
	#include <lights_physical_fragment>
	#include <lights_fragment_begin>
	#include <lights_fragment_maps>
	#include <lights_fragment_end>
	#include <aomap_fragment>
	vec3 totalDiffuse = reflectedLight.directDiffuse + reflectedLight.indirectDiffuse;
	vec3 totalSpecular = reflectedLight.directSpecular + reflectedLight.indirectSpecular;
	#include <transmission_fragment>
	vec3 outgoingLight = totalDiffuse + totalSpecular + totalEmissiveRadiance;
	#ifdef USE_SHEEN
		float sheenEnergyComp = 1.0 - 0.157 * max3( material.sheenColor );
		outgoingLight = outgoingLight * sheenEnergyComp + sheenSpecularDirect + sheenSpecularIndirect;
	#endif
	#ifdef USE_CLEARCOAT
		float dotNVcc = saturate( dot( geometryClearcoatNormal, geometryViewDir ) );
		vec3 Fcc = F_Schlick( material.clearcoatF0, material.clearcoatF90, dotNVcc );
		outgoingLight = outgoingLight * ( 1.0 - material.clearcoat * Fcc ) + ( clearcoatSpecularDirect + clearcoatSpecularIndirect ) * material.clearcoat;
	#endif
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
	#include <dithering_fragment>
}`,yg=`#define TOON
varying vec3 vViewPosition;
#include <common>
#include <batching_pars_vertex>
#include <uv_pars_vertex>
#include <displacementmap_pars_vertex>
#include <color_pars_vertex>
#include <fog_pars_vertex>
#include <normal_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <shadowmap_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	#include <color_vertex>
	#include <morphinstance_vertex>
	#include <morphcolor_vertex>
	#include <batching_vertex>
	#include <beginnormal_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>
	#include <normal_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <displacementmap_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	vViewPosition = - mvPosition.xyz;
	#include <worldpos_vertex>
	#include <shadowmap_vertex>
	#include <fog_vertex>
}`,bg=`#define TOON
uniform vec3 diffuse;
uniform vec3 emissive;
uniform float opacity;
#include <common>
#include <packing>
#include <dithering_pars_fragment>
#include <color_pars_fragment>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <aomap_pars_fragment>
#include <lightmap_pars_fragment>
#include <emissivemap_pars_fragment>
#include <gradientmap_pars_fragment>
#include <fog_pars_fragment>
#include <bsdfs>
#include <lights_pars_begin>
#include <normal_pars_fragment>
#include <lights_toon_pars_fragment>
#include <shadowmap_pars_fragment>
#include <bumpmap_pars_fragment>
#include <normalmap_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	ReflectedLight reflectedLight = ReflectedLight( vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ), vec3( 0.0 ) );
	vec3 totalEmissiveRadiance = emissive;
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <color_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	#include <normal_fragment_begin>
	#include <normal_fragment_maps>
	#include <emissivemap_fragment>
	#include <lights_toon_fragment>
	#include <lights_fragment_begin>
	#include <lights_fragment_maps>
	#include <lights_fragment_end>
	#include <aomap_fragment>
	vec3 outgoingLight = reflectedLight.directDiffuse + reflectedLight.indirectDiffuse + totalEmissiveRadiance;
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
	#include <dithering_fragment>
}`,Eg=`uniform float size;
uniform float scale;
#include <common>
#include <color_pars_vertex>
#include <fog_pars_vertex>
#include <morphtarget_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
#ifdef USE_POINTS_UV
	varying vec2 vUv;
	uniform mat3 uvTransform;
#endif
void main() {
	#ifdef USE_POINTS_UV
		vUv = ( uvTransform * vec3( uv, 1 ) ).xy;
	#endif
	#include <color_vertex>
	#include <morphinstance_vertex>
	#include <morphcolor_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <project_vertex>
	gl_PointSize = size;
	#ifdef USE_SIZEATTENUATION
		bool isPerspective = isPerspectiveMatrix( projectionMatrix );
		if ( isPerspective ) gl_PointSize *= ( scale / - mvPosition.z );
	#endif
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	#include <worldpos_vertex>
	#include <fog_vertex>
}`,Tg=`uniform vec3 diffuse;
uniform float opacity;
#include <common>
#include <color_pars_fragment>
#include <map_particle_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <fog_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	vec3 outgoingLight = vec3( 0.0 );
	#include <logdepthbuf_fragment>
	#include <map_particle_fragment>
	#include <color_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	outgoingLight = diffuseColor.rgb;
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
	#include <premultiplied_alpha_fragment>
}`,wg=`#include <common>
#include <batching_pars_vertex>
#include <fog_pars_vertex>
#include <morphtarget_pars_vertex>
#include <skinning_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <shadowmap_pars_vertex>
void main() {
	#include <batching_vertex>
	#include <beginnormal_vertex>
	#include <morphinstance_vertex>
	#include <morphnormal_vertex>
	#include <skinbase_vertex>
	#include <skinnormal_vertex>
	#include <defaultnormal_vertex>
	#include <begin_vertex>
	#include <morphtarget_vertex>
	#include <skinning_vertex>
	#include <project_vertex>
	#include <logdepthbuf_vertex>
	#include <worldpos_vertex>
	#include <shadowmap_vertex>
	#include <fog_vertex>
}`,Ag=`uniform vec3 color;
uniform float opacity;
#include <common>
#include <packing>
#include <fog_pars_fragment>
#include <bsdfs>
#include <lights_pars_begin>
#include <logdepthbuf_pars_fragment>
#include <shadowmap_pars_fragment>
#include <shadowmask_pars_fragment>
void main() {
	#include <logdepthbuf_fragment>
	gl_FragColor = vec4( color, opacity * ( 1.0 - getShadowMask() ) );
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
}`,Rg=`uniform float rotation;
uniform vec2 center;
#include <common>
#include <uv_pars_vertex>
#include <fog_pars_vertex>
#include <logdepthbuf_pars_vertex>
#include <clipping_planes_pars_vertex>
void main() {
	#include <uv_vertex>
	vec4 mvPosition = modelViewMatrix[ 3 ];
	vec2 scale = vec2( length( modelMatrix[ 0 ].xyz ), length( modelMatrix[ 1 ].xyz ) );
	#ifndef USE_SIZEATTENUATION
		bool isPerspective = isPerspectiveMatrix( projectionMatrix );
		if ( isPerspective ) scale *= - mvPosition.z;
	#endif
	vec2 alignedPosition = ( position.xy - ( center - vec2( 0.5 ) ) ) * scale;
	vec2 rotatedPosition;
	rotatedPosition.x = cos( rotation ) * alignedPosition.x - sin( rotation ) * alignedPosition.y;
	rotatedPosition.y = sin( rotation ) * alignedPosition.x + cos( rotation ) * alignedPosition.y;
	mvPosition.xy += rotatedPosition;
	gl_Position = projectionMatrix * mvPosition;
	#include <logdepthbuf_vertex>
	#include <clipping_planes_vertex>
	#include <fog_vertex>
}`,Cg=`uniform vec3 diffuse;
uniform float opacity;
#include <common>
#include <uv_pars_fragment>
#include <map_pars_fragment>
#include <alphamap_pars_fragment>
#include <alphatest_pars_fragment>
#include <alphahash_pars_fragment>
#include <fog_pars_fragment>
#include <logdepthbuf_pars_fragment>
#include <clipping_planes_pars_fragment>
void main() {
	vec4 diffuseColor = vec4( diffuse, opacity );
	#include <clipping_planes_fragment>
	vec3 outgoingLight = vec3( 0.0 );
	#include <logdepthbuf_fragment>
	#include <map_fragment>
	#include <alphamap_fragment>
	#include <alphatest_fragment>
	#include <alphahash_fragment>
	outgoingLight = diffuseColor.rgb;
	#include <opaque_fragment>
	#include <tonemapping_fragment>
	#include <colorspace_fragment>
	#include <fog_fragment>
}`,Ke={alphahash_fragment:Kf,alphahash_pars_fragment:Jf,alphamap_fragment:Qf,alphamap_pars_fragment:ep,alphatest_fragment:tp,alphatest_pars_fragment:np,aomap_fragment:ip,aomap_pars_fragment:sp,batching_pars_vertex:rp,batching_vertex:ap,begin_vertex:op,beginnormal_vertex:lp,bsdfs:cp,iridescence_fragment:up,bumpmap_pars_fragment:dp,clipping_planes_fragment:hp,clipping_planes_pars_fragment:fp,clipping_planes_pars_vertex:pp,clipping_planes_vertex:mp,color_fragment:gp,color_pars_fragment:_p,color_pars_vertex:vp,color_vertex:xp,common:Mp,cube_uv_reflection_fragment:Sp,defaultnormal_vertex:yp,displacementmap_pars_vertex:bp,displacementmap_vertex:Ep,emissivemap_fragment:Tp,emissivemap_pars_fragment:wp,colorspace_fragment:Ap,colorspace_pars_fragment:Rp,envmap_fragment:Cp,envmap_common_pars_fragment:Pp,envmap_pars_fragment:Lp,envmap_pars_vertex:Dp,envmap_physical_pars_fragment:Gp,envmap_vertex:Ip,fog_vertex:Up,fog_pars_vertex:kp,fog_fragment:Np,fog_pars_fragment:Fp,gradientmap_pars_fragment:Op,lightmap_pars_fragment:Bp,lights_lambert_fragment:zp,lights_lambert_pars_fragment:Vp,lights_pars_begin:Hp,lights_toon_fragment:Wp,lights_toon_pars_fragment:$p,lights_phong_fragment:Xp,lights_phong_pars_fragment:qp,lights_physical_fragment:Yp,lights_physical_pars_fragment:jp,lights_fragment_begin:Zp,lights_fragment_maps:Kp,lights_fragment_end:Jp,logdepthbuf_fragment:Qp,logdepthbuf_pars_fragment:em,logdepthbuf_pars_vertex:tm,logdepthbuf_vertex:nm,map_fragment:im,map_pars_fragment:sm,map_particle_fragment:rm,map_particle_pars_fragment:am,metalnessmap_fragment:om,metalnessmap_pars_fragment:lm,morphinstance_vertex:cm,morphcolor_vertex:um,morphnormal_vertex:dm,morphtarget_pars_vertex:hm,morphtarget_vertex:fm,normal_fragment_begin:pm,normal_fragment_maps:mm,normal_pars_fragment:gm,normal_pars_vertex:_m,normal_vertex:vm,normalmap_pars_fragment:xm,clearcoat_normal_fragment_begin:Mm,clearcoat_normal_fragment_maps:Sm,clearcoat_pars_fragment:ym,iridescence_pars_fragment:bm,opaque_fragment:Em,packing:Tm,premultiplied_alpha_fragment:wm,project_vertex:Am,dithering_fragment:Rm,dithering_pars_fragment:Cm,roughnessmap_fragment:Pm,roughnessmap_pars_fragment:Lm,shadowmap_pars_fragment:Dm,shadowmap_pars_vertex:Im,shadowmap_vertex:Um,shadowmask_pars_fragment:km,skinbase_vertex:Nm,skinning_pars_vertex:Fm,skinning_vertex:Om,skinnormal_vertex:Bm,specularmap_fragment:zm,specularmap_pars_fragment:Vm,tonemapping_fragment:Hm,tonemapping_pars_fragment:Gm,transmission_fragment:Wm,transmission_pars_fragment:$m,uv_pars_fragment:Xm,uv_pars_vertex:qm,uv_vertex:Ym,worldpos_vertex:jm,background_vert:Zm,background_frag:Km,backgroundCube_vert:Jm,backgroundCube_frag:Qm,cube_vert:eg,cube_frag:tg,depth_vert:ng,depth_frag:ig,distanceRGBA_vert:sg,distanceRGBA_frag:rg,equirect_vert:ag,equirect_frag:og,linedashed_vert:lg,linedashed_frag:cg,meshbasic_vert:ug,meshbasic_frag:dg,meshlambert_vert:hg,meshlambert_frag:fg,meshmatcap_vert:pg,meshmatcap_frag:mg,meshnormal_vert:gg,meshnormal_frag:_g,meshphong_vert:vg,meshphong_frag:xg,meshphysical_vert:Mg,meshphysical_frag:Sg,meshtoon_vert:yg,meshtoon_frag:bg,points_vert:Eg,points_frag:Tg,shadow_vert:wg,shadow_frag:Ag,sprite_vert:Rg,sprite_frag:Cg},fe={common:{diffuse:{value:new Le(16777215)},opacity:{value:1},map:{value:null},mapTransform:{value:new Ze},alphaMap:{value:null},alphaMapTransform:{value:new Ze},alphaTest:{value:0}},specularmap:{specularMap:{value:null},specularMapTransform:{value:new Ze}},envmap:{envMap:{value:null},envMapRotation:{value:new Ze},flipEnvMap:{value:-1},reflectivity:{value:1},ior:{value:1.5},refractionRatio:{value:.98}},aomap:{aoMap:{value:null},aoMapIntensity:{value:1},aoMapTransform:{value:new Ze}},lightmap:{lightMap:{value:null},lightMapIntensity:{value:1},lightMapTransform:{value:new Ze}},bumpmap:{bumpMap:{value:null},bumpMapTransform:{value:new Ze},bumpScale:{value:1}},normalmap:{normalMap:{value:null},normalMapTransform:{value:new Ze},normalScale:{value:new et(1,1)}},displacementmap:{displacementMap:{value:null},displacementMapTransform:{value:new Ze},displacementScale:{value:1},displacementBias:{value:0}},emissivemap:{emissiveMap:{value:null},emissiveMapTransform:{value:new Ze}},metalnessmap:{metalnessMap:{value:null},metalnessMapTransform:{value:new Ze}},roughnessmap:{roughnessMap:{value:null},roughnessMapTransform:{value:new Ze}},gradientmap:{gradientMap:{value:null}},fog:{fogDensity:{value:25e-5},fogNear:{value:1},fogFar:{value:2e3},fogColor:{value:new Le(16777215)}},lights:{ambientLightColor:{value:[]},lightProbe:{value:[]},directionalLights:{value:[],properties:{direction:{},color:{}}},directionalLightShadows:{value:[],properties:{shadowIntensity:1,shadowBias:{},shadowNormalBias:{},shadowRadius:{},shadowMapSize:{}}},directionalShadowMap:{value:[]},directionalShadowMatrix:{value:[]},spotLights:{value:[],properties:{color:{},position:{},direction:{},distance:{},coneCos:{},penumbraCos:{},decay:{}}},spotLightShadows:{value:[],properties:{shadowIntensity:1,shadowBias:{},shadowNormalBias:{},shadowRadius:{},shadowMapSize:{}}},spotLightMap:{value:[]},spotShadowMap:{value:[]},spotLightMatrix:{value:[]},pointLights:{value:[],properties:{color:{},position:{},decay:{},distance:{}}},pointLightShadows:{value:[],properties:{shadowIntensity:1,shadowBias:{},shadowNormalBias:{},shadowRadius:{},shadowMapSize:{},shadowCameraNear:{},shadowCameraFar:{}}},pointShadowMap:{value:[]},pointShadowMatrix:{value:[]},hemisphereLights:{value:[],properties:{direction:{},skyColor:{},groundColor:{}}},rectAreaLights:{value:[],properties:{color:{},position:{},width:{},height:{}}},ltc_1:{value:null},ltc_2:{value:null}},points:{diffuse:{value:new Le(16777215)},opacity:{value:1},size:{value:1},scale:{value:1},map:{value:null},alphaMap:{value:null},alphaMapTransform:{value:new Ze},alphaTest:{value:0},uvTransform:{value:new Ze}},sprite:{diffuse:{value:new Le(16777215)},opacity:{value:1},center:{value:new et(.5,.5)},rotation:{value:0},map:{value:null},mapTransform:{value:new Ze},alphaMap:{value:null},alphaMapTransform:{value:new Ze},alphaTest:{value:0}}},gn={basic:{uniforms:Gt([fe.common,fe.specularmap,fe.envmap,fe.aomap,fe.lightmap,fe.fog]),vertexShader:Ke.meshbasic_vert,fragmentShader:Ke.meshbasic_frag},lambert:{uniforms:Gt([fe.common,fe.specularmap,fe.envmap,fe.aomap,fe.lightmap,fe.emissivemap,fe.bumpmap,fe.normalmap,fe.displacementmap,fe.fog,fe.lights,{emissive:{value:new Le(0)}}]),vertexShader:Ke.meshlambert_vert,fragmentShader:Ke.meshlambert_frag},phong:{uniforms:Gt([fe.common,fe.specularmap,fe.envmap,fe.aomap,fe.lightmap,fe.emissivemap,fe.bumpmap,fe.normalmap,fe.displacementmap,fe.fog,fe.lights,{emissive:{value:new Le(0)},specular:{value:new Le(1118481)},shininess:{value:30}}]),vertexShader:Ke.meshphong_vert,fragmentShader:Ke.meshphong_frag},standard:{uniforms:Gt([fe.common,fe.envmap,fe.aomap,fe.lightmap,fe.emissivemap,fe.bumpmap,fe.normalmap,fe.displacementmap,fe.roughnessmap,fe.metalnessmap,fe.fog,fe.lights,{emissive:{value:new Le(0)},roughness:{value:1},metalness:{value:0},envMapIntensity:{value:1}}]),vertexShader:Ke.meshphysical_vert,fragmentShader:Ke.meshphysical_frag},toon:{uniforms:Gt([fe.common,fe.aomap,fe.lightmap,fe.emissivemap,fe.bumpmap,fe.normalmap,fe.displacementmap,fe.gradientmap,fe.fog,fe.lights,{emissive:{value:new Le(0)}}]),vertexShader:Ke.meshtoon_vert,fragmentShader:Ke.meshtoon_frag},matcap:{uniforms:Gt([fe.common,fe.bumpmap,fe.normalmap,fe.displacementmap,fe.fog,{matcap:{value:null}}]),vertexShader:Ke.meshmatcap_vert,fragmentShader:Ke.meshmatcap_frag},points:{uniforms:Gt([fe.points,fe.fog]),vertexShader:Ke.points_vert,fragmentShader:Ke.points_frag},dashed:{uniforms:Gt([fe.common,fe.fog,{scale:{value:1},dashSize:{value:1},totalSize:{value:2}}]),vertexShader:Ke.linedashed_vert,fragmentShader:Ke.linedashed_frag},depth:{uniforms:Gt([fe.common,fe.displacementmap]),vertexShader:Ke.depth_vert,fragmentShader:Ke.depth_frag},normal:{uniforms:Gt([fe.common,fe.bumpmap,fe.normalmap,fe.displacementmap,{opacity:{value:1}}]),vertexShader:Ke.meshnormal_vert,fragmentShader:Ke.meshnormal_frag},sprite:{uniforms:Gt([fe.sprite,fe.fog]),vertexShader:Ke.sprite_vert,fragmentShader:Ke.sprite_frag},background:{uniforms:{uvTransform:{value:new Ze},t2D:{value:null},backgroundIntensity:{value:1}},vertexShader:Ke.background_vert,fragmentShader:Ke.background_frag},backgroundCube:{uniforms:{envMap:{value:null},flipEnvMap:{value:-1},backgroundBlurriness:{value:0},backgroundIntensity:{value:1},backgroundRotation:{value:new Ze}},vertexShader:Ke.backgroundCube_vert,fragmentShader:Ke.backgroundCube_frag},cube:{uniforms:{tCube:{value:null},tFlip:{value:-1},opacity:{value:1}},vertexShader:Ke.cube_vert,fragmentShader:Ke.cube_frag},equirect:{uniforms:{tEquirect:{value:null}},vertexShader:Ke.equirect_vert,fragmentShader:Ke.equirect_frag},distanceRGBA:{uniforms:Gt([fe.common,fe.displacementmap,{referencePosition:{value:new O},nearDistance:{value:1},farDistance:{value:1e3}}]),vertexShader:Ke.distanceRGBA_vert,fragmentShader:Ke.distanceRGBA_frag},shadow:{uniforms:Gt([fe.lights,fe.fog,{color:{value:new Le(0)},opacity:{value:1}}]),vertexShader:Ke.shadow_vert,fragmentShader:Ke.shadow_frag}};gn.physical={uniforms:Gt([gn.standard.uniforms,{clearcoat:{value:0},clearcoatMap:{value:null},clearcoatMapTransform:{value:new Ze},clearcoatNormalMap:{value:null},clearcoatNormalMapTransform:{value:new Ze},clearcoatNormalScale:{value:new et(1,1)},clearcoatRoughness:{value:0},clearcoatRoughnessMap:{value:null},clearcoatRoughnessMapTransform:{value:new Ze},dispersion:{value:0},iridescence:{value:0},iridescenceMap:{value:null},iridescenceMapTransform:{value:new Ze},iridescenceIOR:{value:1.3},iridescenceThicknessMinimum:{value:100},iridescenceThicknessMaximum:{value:400},iridescenceThicknessMap:{value:null},iridescenceThicknessMapTransform:{value:new Ze},sheen:{value:0},sheenColor:{value:new Le(0)},sheenColorMap:{value:null},sheenColorMapTransform:{value:new Ze},sheenRoughness:{value:1},sheenRoughnessMap:{value:null},sheenRoughnessMapTransform:{value:new Ze},transmission:{value:0},transmissionMap:{value:null},transmissionMapTransform:{value:new Ze},transmissionSamplerSize:{value:new et},transmissionSamplerMap:{value:null},thickness:{value:0},thicknessMap:{value:null},thicknessMapTransform:{value:new Ze},attenuationDistance:{value:0},attenuationColor:{value:new Le(0)},specularColor:{value:new Le(1,1,1)},specularColorMap:{value:null},specularColorMapTransform:{value:new Ze},specularIntensity:{value:1},specularIntensityMap:{value:null},specularIntensityMapTransform:{value:new Ze},anisotropyVector:{value:new et},anisotropyMap:{value:null},anisotropyMapTransform:{value:new Ze}}]),vertexShader:Ke.meshphysical_vert,fragmentShader:Ke.meshphysical_frag};const Ws={r:0,b:0,g:0},ni=new Sn,Pg=new bt;function Lg(i,e,t,n,s,r,a){const o=new Le(0);let l=r===!0?0:1,c,u,d=null,h=0,p=null;function g(b){let v=b.isScene===!0?b.background:null;return v&&v.isTexture&&(v=(b.backgroundBlurriness>0?t:e).get(v)),v}function _(b){let v=!1;const T=g(b);T===null?f(o,l):T&&T.isColor&&(f(T,1),v=!0);const A=i.xr.getEnvironmentBlendMode();A==="additive"?n.buffers.color.setClear(0,0,0,1,a):A==="alpha-blend"&&n.buffers.color.setClear(0,0,0,0,a),(i.autoClear||v)&&(n.buffers.depth.setTest(!0),n.buffers.depth.setMask(!0),n.buffers.color.setMask(!0),i.clear(i.autoClearColor,i.autoClearDepth,i.autoClearStencil))}function m(b,v){const T=g(v);T&&(T.isCubeTexture||T.mapping===_r)?(u===void 0&&(u=new kt(new vs(1,1,1),new on({name:"BackgroundCubeMaterial",uniforms:Gi(gn.backgroundCube.uniforms),vertexShader:gn.backgroundCube.vertexShader,fragmentShader:gn.backgroundCube.fragmentShader,side:Yt,depthTest:!1,depthWrite:!1,fog:!1,allowOverride:!1})),u.geometry.deleteAttribute("normal"),u.geometry.deleteAttribute("uv"),u.onBeforeRender=function(A,C,I){this.matrixWorld.copyPosition(I.matrixWorld)},Object.defineProperty(u.material,"envMap",{get:function(){return this.uniforms.envMap.value}}),s.update(u)),ni.copy(v.backgroundRotation),ni.x*=-1,ni.y*=-1,ni.z*=-1,T.isCubeTexture&&T.isRenderTargetTexture===!1&&(ni.y*=-1,ni.z*=-1),u.material.uniforms.envMap.value=T,u.material.uniforms.flipEnvMap.value=T.isCubeTexture&&T.isRenderTargetTexture===!1?-1:1,u.material.uniforms.backgroundBlurriness.value=v.backgroundBlurriness,u.material.uniforms.backgroundIntensity.value=v.backgroundIntensity,u.material.uniforms.backgroundRotation.value.setFromMatrix4(Pg.makeRotationFromEuler(ni)),u.material.toneMapped=lt.getTransfer(T.colorSpace)!==mt,(d!==T||h!==T.version||p!==i.toneMapping)&&(u.material.needsUpdate=!0,d=T,h=T.version,p=i.toneMapping),u.layers.enableAll(),b.unshift(u,u.geometry,u.material,0,0,null)):T&&T.isTexture&&(c===void 0&&(c=new kt(new Yn(2,2),new on({name:"BackgroundMaterial",uniforms:Gi(gn.background.uniforms),vertexShader:gn.background.vertexShader,fragmentShader:gn.background.fragmentShader,side:qn,depthTest:!1,depthWrite:!1,fog:!1,allowOverride:!1})),c.geometry.deleteAttribute("normal"),Object.defineProperty(c.material,"map",{get:function(){return this.uniforms.t2D.value}}),s.update(c)),c.material.uniforms.t2D.value=T,c.material.uniforms.backgroundIntensity.value=v.backgroundIntensity,c.material.toneMapped=lt.getTransfer(T.colorSpace)!==mt,T.matrixAutoUpdate===!0&&T.updateMatrix(),c.material.uniforms.uvTransform.value.copy(T.matrix),(d!==T||h!==T.version||p!==i.toneMapping)&&(c.material.needsUpdate=!0,d=T,h=T.version,p=i.toneMapping),c.layers.enableAll(),b.unshift(c,c.geometry,c.material,0,0,null))}function f(b,v){b.getRGB(Ws,Nc(i)),n.buffers.color.setClear(Ws.r,Ws.g,Ws.b,v,a)}function E(){u!==void 0&&(u.geometry.dispose(),u.material.dispose(),u=void 0),c!==void 0&&(c.geometry.dispose(),c.material.dispose(),c=void 0)}return{getClearColor:function(){return o},setClearColor:function(b,v=1){o.set(b),l=v,f(o,l)},getClearAlpha:function(){return l},setClearAlpha:function(b){l=b,f(o,l)},render:_,addToRenderList:m,dispose:E}}function Dg(i,e){const t=i.getParameter(i.MAX_VERTEX_ATTRIBS),n={},s=h(null);let r=s,a=!1;function o(S,P,$,q,Y){let X=!1;const B=d(q,$,P);r!==B&&(r=B,c(r.object)),X=p(S,q,$,Y),X&&g(S,q,$,Y),Y!==null&&e.update(Y,i.ELEMENT_ARRAY_BUFFER),(X||a)&&(a=!1,v(S,P,$,q),Y!==null&&i.bindBuffer(i.ELEMENT_ARRAY_BUFFER,e.get(Y).buffer))}function l(){return i.createVertexArray()}function c(S){return i.bindVertexArray(S)}function u(S){return i.deleteVertexArray(S)}function d(S,P,$){const q=$.wireframe===!0;let Y=n[S.id];Y===void 0&&(Y={},n[S.id]=Y);let X=Y[P.id];X===void 0&&(X={},Y[P.id]=X);let B=X[q];return B===void 0&&(B=h(l()),X[q]=B),B}function h(S){const P=[],$=[],q=[];for(let Y=0;Y<t;Y++)P[Y]=0,$[Y]=0,q[Y]=0;return{geometry:null,program:null,wireframe:!1,newAttributes:P,enabledAttributes:$,attributeDivisors:q,object:S,attributes:{},index:null}}function p(S,P,$,q){const Y=r.attributes,X=P.attributes;let B=0;const Q=$.getAttributes();for(const G in Q)if(Q[G].location>=0){const de=Y[G];let Ee=X[G];if(Ee===void 0&&(G==="instanceMatrix"&&S.instanceMatrix&&(Ee=S.instanceMatrix),G==="instanceColor"&&S.instanceColor&&(Ee=S.instanceColor)),de===void 0||de.attribute!==Ee||Ee&&de.data!==Ee.data)return!0;B++}return r.attributesNum!==B||r.index!==q}function g(S,P,$,q){const Y={},X=P.attributes;let B=0;const Q=$.getAttributes();for(const G in Q)if(Q[G].location>=0){let de=X[G];de===void 0&&(G==="instanceMatrix"&&S.instanceMatrix&&(de=S.instanceMatrix),G==="instanceColor"&&S.instanceColor&&(de=S.instanceColor));const Ee={};Ee.attribute=de,de&&de.data&&(Ee.data=de.data),Y[G]=Ee,B++}r.attributes=Y,r.attributesNum=B,r.index=q}function _(){const S=r.newAttributes;for(let P=0,$=S.length;P<$;P++)S[P]=0}function m(S){f(S,0)}function f(S,P){const $=r.newAttributes,q=r.enabledAttributes,Y=r.attributeDivisors;$[S]=1,q[S]===0&&(i.enableVertexAttribArray(S),q[S]=1),Y[S]!==P&&(i.vertexAttribDivisor(S,P),Y[S]=P)}function E(){const S=r.newAttributes,P=r.enabledAttributes;for(let $=0,q=P.length;$<q;$++)P[$]!==S[$]&&(i.disableVertexAttribArray($),P[$]=0)}function b(S,P,$,q,Y,X,B){B===!0?i.vertexAttribIPointer(S,P,$,Y,X):i.vertexAttribPointer(S,P,$,q,Y,X)}function v(S,P,$,q){_();const Y=q.attributes,X=$.getAttributes(),B=P.defaultAttributeValues;for(const Q in X){const G=X[Q];if(G.location>=0){let ie=Y[Q];if(ie===void 0&&(Q==="instanceMatrix"&&S.instanceMatrix&&(ie=S.instanceMatrix),Q==="instanceColor"&&S.instanceColor&&(ie=S.instanceColor)),ie!==void 0){const de=ie.normalized,Ee=ie.itemSize,Oe=e.get(ie);if(Oe===void 0)continue;const at=Oe.buffer,He=Oe.type,j=Oe.bytesPerElement,he=He===i.INT||He===i.UNSIGNED_INT||ie.gpuType===xo;if(ie.isInterleavedBufferAttribute){const le=ie.data,Be=le.stride,Ue=ie.offset;if(le.isInstancedInterleavedBuffer){for(let $e=0;$e<G.locationSize;$e++)f(G.location+$e,le.meshPerAttribute);S.isInstancedMesh!==!0&&q._maxInstanceCount===void 0&&(q._maxInstanceCount=le.meshPerAttribute*le.count)}else for(let $e=0;$e<G.locationSize;$e++)m(G.location+$e);i.bindBuffer(i.ARRAY_BUFFER,at);for(let $e=0;$e<G.locationSize;$e++)b(G.location+$e,Ee/G.locationSize,He,de,Be*j,(Ue+Ee/G.locationSize*$e)*j,he)}else{if(ie.isInstancedBufferAttribute){for(let le=0;le<G.locationSize;le++)f(G.location+le,ie.meshPerAttribute);S.isInstancedMesh!==!0&&q._maxInstanceCount===void 0&&(q._maxInstanceCount=ie.meshPerAttribute*ie.count)}else for(let le=0;le<G.locationSize;le++)m(G.location+le);i.bindBuffer(i.ARRAY_BUFFER,at);for(let le=0;le<G.locationSize;le++)b(G.location+le,Ee/G.locationSize,He,de,Ee*j,Ee/G.locationSize*le*j,he)}}else if(B!==void 0){const de=B[Q];if(de!==void 0)switch(de.length){case 2:i.vertexAttrib2fv(G.location,de);break;case 3:i.vertexAttrib3fv(G.location,de);break;case 4:i.vertexAttrib4fv(G.location,de);break;default:i.vertexAttrib1fv(G.location,de)}}}}E()}function T(){I();for(const S in n){const P=n[S];for(const $ in P){const q=P[$];for(const Y in q)u(q[Y].object),delete q[Y];delete P[$]}delete n[S]}}function A(S){if(n[S.id]===void 0)return;const P=n[S.id];for(const $ in P){const q=P[$];for(const Y in q)u(q[Y].object),delete q[Y];delete P[$]}delete n[S.id]}function C(S){for(const P in n){const $=n[P];if($[S.id]===void 0)continue;const q=$[S.id];for(const Y in q)u(q[Y].object),delete q[Y];delete $[S.id]}}function I(){y(),a=!0,r!==s&&(r=s,c(r.object))}function y(){s.geometry=null,s.program=null,s.wireframe=!1}return{setup:o,reset:I,resetDefaultState:y,dispose:T,releaseStatesOfGeometry:A,releaseStatesOfProgram:C,initAttributes:_,enableAttribute:m,disableUnusedAttributes:E}}function Ig(i,e,t){let n;function s(c){n=c}function r(c,u){i.drawArrays(n,c,u),t.update(u,n,1)}function a(c,u,d){d!==0&&(i.drawArraysInstanced(n,c,u,d),t.update(u,n,d))}function o(c,u,d){if(d===0)return;e.get("WEBGL_multi_draw").multiDrawArraysWEBGL(n,c,0,u,0,d);let p=0;for(let g=0;g<d;g++)p+=u[g];t.update(p,n,1)}function l(c,u,d,h){if(d===0)return;const p=e.get("WEBGL_multi_draw");if(p===null)for(let g=0;g<c.length;g++)a(c[g],u[g],h[g]);else{p.multiDrawArraysInstancedWEBGL(n,c,0,u,0,h,0,d);let g=0;for(let _=0;_<d;_++)g+=u[_]*h[_];t.update(g,n,1)}}this.setMode=s,this.render=r,this.renderInstances=a,this.renderMultiDraw=o,this.renderMultiDrawInstances=l}function Ug(i,e,t,n){let s;function r(){if(s!==void 0)return s;if(e.has("EXT_texture_filter_anisotropic")===!0){const C=e.get("EXT_texture_filter_anisotropic");s=i.getParameter(C.MAX_TEXTURE_MAX_ANISOTROPY_EXT)}else s=0;return s}function a(C){return!(C!==hn&&n.convert(C)!==i.getParameter(i.IMPLEMENTATION_COLOR_READ_FORMAT))}function o(C){const I=C===ps&&(e.has("EXT_color_buffer_half_float")||e.has("EXT_color_buffer_float"));return!(C!==Mn&&n.convert(C)!==i.getParameter(i.IMPLEMENTATION_COLOR_READ_TYPE)&&C!==Pn&&!I)}function l(C){if(C==="highp"){if(i.getShaderPrecisionFormat(i.VERTEX_SHADER,i.HIGH_FLOAT).precision>0&&i.getShaderPrecisionFormat(i.FRAGMENT_SHADER,i.HIGH_FLOAT).precision>0)return"highp";C="mediump"}return C==="mediump"&&i.getShaderPrecisionFormat(i.VERTEX_SHADER,i.MEDIUM_FLOAT).precision>0&&i.getShaderPrecisionFormat(i.FRAGMENT_SHADER,i.MEDIUM_FLOAT).precision>0?"mediump":"lowp"}let c=t.precision!==void 0?t.precision:"highp";const u=l(c);u!==c&&(console.warn("THREE.WebGLRenderer:",c,"not supported, using",u,"instead."),c=u);const d=t.logarithmicDepthBuffer===!0,h=t.reversedDepthBuffer===!0&&e.has("EXT_clip_control"),p=i.getParameter(i.MAX_TEXTURE_IMAGE_UNITS),g=i.getParameter(i.MAX_VERTEX_TEXTURE_IMAGE_UNITS),_=i.getParameter(i.MAX_TEXTURE_SIZE),m=i.getParameter(i.MAX_CUBE_MAP_TEXTURE_SIZE),f=i.getParameter(i.MAX_VERTEX_ATTRIBS),E=i.getParameter(i.MAX_VERTEX_UNIFORM_VECTORS),b=i.getParameter(i.MAX_VARYING_VECTORS),v=i.getParameter(i.MAX_FRAGMENT_UNIFORM_VECTORS),T=g>0,A=i.getParameter(i.MAX_SAMPLES);return{isWebGL2:!0,getMaxAnisotropy:r,getMaxPrecision:l,textureFormatReadable:a,textureTypeReadable:o,precision:c,logarithmicDepthBuffer:d,reversedDepthBuffer:h,maxTextures:p,maxVertexTextures:g,maxTextureSize:_,maxCubemapSize:m,maxAttributes:f,maxVertexUniforms:E,maxVaryings:b,maxFragmentUniforms:v,vertexTextures:T,maxSamples:A}}function kg(i){const e=this;let t=null,n=0,s=!1,r=!1;const a=new si,o=new Ze,l={value:null,needsUpdate:!1};this.uniform=l,this.numPlanes=0,this.numIntersection=0,this.init=function(d,h){const p=d.length!==0||h||n!==0||s;return s=h,n=d.length,p},this.beginShadows=function(){r=!0,u(null)},this.endShadows=function(){r=!1},this.setGlobalState=function(d,h){t=u(d,h,0)},this.setState=function(d,h,p){const g=d.clippingPlanes,_=d.clipIntersection,m=d.clipShadows,f=i.get(d);if(!s||g===null||g.length===0||r&&!m)r?u(null):c();else{const E=r?0:n,b=E*4;let v=f.clippingState||null;l.value=v,v=u(g,h,b,p);for(let T=0;T!==b;++T)v[T]=t[T];f.clippingState=v,this.numIntersection=_?this.numPlanes:0,this.numPlanes+=E}};function c(){l.value!==t&&(l.value=t,l.needsUpdate=n>0),e.numPlanes=n,e.numIntersection=0}function u(d,h,p,g){const _=d!==null?d.length:0;let m=null;if(_!==0){if(m=l.value,g!==!0||m===null){const f=p+_*4,E=h.matrixWorldInverse;o.getNormalMatrix(E),(m===null||m.length<f)&&(m=new Float32Array(f));for(let b=0,v=p;b!==_;++b,v+=4)a.copy(d[b]).applyMatrix4(E,o),a.normal.toArray(m,v),m[v+3]=a.constant}l.value=m,l.needsUpdate=!0}return e.numPlanes=_,e.numIntersection=0,m}}function Ng(i){let e=new WeakMap;function t(a,o){return o===Aa?a.mapping=zi:o===Ra&&(a.mapping=Vi),a}function n(a){if(a&&a.isTexture){const o=a.mapping;if(o===Aa||o===Ra)if(e.has(a)){const l=e.get(a).texture;return t(l,a.mapping)}else{const l=a.image;if(l&&l.height>0){const c=new Pf(l.height);return c.fromEquirectangularTexture(i,a),e.set(a,c),a.addEventListener("dispose",s),t(c.texture,a.mapping)}else return null}}return a}function s(a){const o=a.target;o.removeEventListener("dispose",s);const l=e.get(o);l!==void 0&&(e.delete(o),l.dispose())}function r(){e=new WeakMap}return{get:n,dispose:r}}const Ii=4,Il=[.125,.215,.35,.446,.526,.582],oi=20,ta=new Gc,Ul=new Le;let na=null,ia=0,sa=0,ra=!1;const ri=(1+Math.sqrt(5))/2,Li=1/ri,kl=[new O(-ri,Li,0),new O(ri,Li,0),new O(-Li,0,ri),new O(Li,0,ri),new O(0,ri,-Li),new O(0,ri,Li),new O(-1,1,-1),new O(1,1,-1),new O(-1,1,1),new O(1,1,1)],Fg=new O;class Nl{constructor(e){this._renderer=e,this._pingPongRenderTarget=null,this._lodMax=0,this._cubeSize=0,this._lodPlanes=[],this._sizeLods=[],this._sigmas=[],this._blurMaterial=null,this._cubemapMaterial=null,this._equirectMaterial=null,this._compileMaterial(this._blurMaterial)}fromScene(e,t=0,n=.1,s=100,r={}){const{size:a=256,position:o=Fg}=r;na=this._renderer.getRenderTarget(),ia=this._renderer.getActiveCubeFace(),sa=this._renderer.getActiveMipmapLevel(),ra=this._renderer.xr.enabled,this._renderer.xr.enabled=!1,this._setSize(a);const l=this._allocateTargets();return l.depthBuffer=!0,this._sceneToCubeUV(e,n,s,l,o),t>0&&this._blur(l,0,0,t),this._applyPMREM(l),this._cleanup(l),l}fromEquirectangular(e,t=null){return this._fromTexture(e,t)}fromCubemap(e,t=null){return this._fromTexture(e,t)}compileCubemapShader(){this._cubemapMaterial===null&&(this._cubemapMaterial=Bl(),this._compileMaterial(this._cubemapMaterial))}compileEquirectangularShader(){this._equirectMaterial===null&&(this._equirectMaterial=Ol(),this._compileMaterial(this._equirectMaterial))}dispose(){this._dispose(),this._cubemapMaterial!==null&&this._cubemapMaterial.dispose(),this._equirectMaterial!==null&&this._equirectMaterial.dispose()}_setSize(e){this._lodMax=Math.floor(Math.log2(e)),this._cubeSize=Math.pow(2,this._lodMax)}_dispose(){this._blurMaterial!==null&&this._blurMaterial.dispose(),this._pingPongRenderTarget!==null&&this._pingPongRenderTarget.dispose();for(let e=0;e<this._lodPlanes.length;e++)this._lodPlanes[e].dispose()}_cleanup(e){this._renderer.setRenderTarget(na,ia,sa),this._renderer.xr.enabled=ra,e.scissorTest=!1,$s(e,0,0,e.width,e.height)}_fromTexture(e,t){e.mapping===zi||e.mapping===Vi?this._setSize(e.image.length===0?16:e.image[0].width||e.image[0].image.width):this._setSize(e.image.width/4),na=this._renderer.getRenderTarget(),ia=this._renderer.getActiveCubeFace(),sa=this._renderer.getActiveMipmapLevel(),ra=this._renderer.xr.enabled,this._renderer.xr.enabled=!1;const n=t||this._allocateTargets();return this._textureToCubeUV(e,n),this._applyPMREM(n),this._cleanup(n),n}_allocateTargets(){const e=3*Math.max(this._cubeSize,112),t=4*this._cubeSize,n={magFilter:an,minFilter:an,generateMipmaps:!1,type:ps,format:hn,colorSpace:Hi,depthBuffer:!1},s=Fl(e,t,n);if(this._pingPongRenderTarget===null||this._pingPongRenderTarget.width!==e||this._pingPongRenderTarget.height!==t){this._pingPongRenderTarget!==null&&this._dispose(),this._pingPongRenderTarget=Fl(e,t,n);const{_lodMax:r}=this;({sizeLods:this._sizeLods,lodPlanes:this._lodPlanes,sigmas:this._sigmas}=Og(r)),this._blurMaterial=Bg(r,e,t)}return s}_compileMaterial(e){const t=new kt(this._lodPlanes[0],e);this._renderer.compile(t,ta)}_sceneToCubeUV(e,t,n,s,r){const l=new Wt(90,1,t,n),c=[1,-1,1,1,1,1],u=[1,1,1,-1,-1,-1],d=this._renderer,h=d.autoClear,p=d.toneMapping;d.getClearColor(Ul),d.toneMapping=$n,d.autoClear=!1,d.state.buffers.depth.getReversed()&&(d.setRenderTarget(s),d.clearDepth(),d.setRenderTarget(null));const _=new hs({name:"PMREM.Background",side:Yt,depthWrite:!1,depthTest:!1}),m=new kt(new vs,_);let f=!1;const E=e.background;E?E.isColor&&(_.color.copy(E),e.background=null,f=!0):(_.color.copy(Ul),f=!0);for(let b=0;b<6;b++){const v=b%3;v===0?(l.up.set(0,c[b],0),l.position.set(r.x,r.y,r.z),l.lookAt(r.x+u[b],r.y,r.z)):v===1?(l.up.set(0,0,c[b]),l.position.set(r.x,r.y,r.z),l.lookAt(r.x,r.y+u[b],r.z)):(l.up.set(0,c[b],0),l.position.set(r.x,r.y,r.z),l.lookAt(r.x,r.y,r.z+u[b]));const T=this._cubeSize;$s(s,v*T,b>2?T:0,T,T),d.setRenderTarget(s),f&&d.render(m,l),d.render(e,l)}m.geometry.dispose(),m.material.dispose(),d.toneMapping=p,d.autoClear=h,e.background=E}_textureToCubeUV(e,t){const n=this._renderer,s=e.mapping===zi||e.mapping===Vi;s?(this._cubemapMaterial===null&&(this._cubemapMaterial=Bl()),this._cubemapMaterial.uniforms.flipEnvMap.value=e.isRenderTargetTexture===!1?-1:1):this._equirectMaterial===null&&(this._equirectMaterial=Ol());const r=s?this._cubemapMaterial:this._equirectMaterial,a=new kt(this._lodPlanes[0],r),o=r.uniforms;o.envMap.value=e;const l=this._cubeSize;$s(t,0,0,3*l,2*l),n.setRenderTarget(t),n.render(a,ta)}_applyPMREM(e){const t=this._renderer,n=t.autoClear;t.autoClear=!1;const s=this._lodPlanes.length;for(let r=1;r<s;r++){const a=Math.sqrt(this._sigmas[r]*this._sigmas[r]-this._sigmas[r-1]*this._sigmas[r-1]),o=kl[(s-r-1)%kl.length];this._blur(e,r-1,r,a,o)}t.autoClear=n}_blur(e,t,n,s,r){const a=this._pingPongRenderTarget;this._halfBlur(e,a,t,n,s,"latitudinal",r),this._halfBlur(a,e,n,n,s,"longitudinal",r)}_halfBlur(e,t,n,s,r,a,o){const l=this._renderer,c=this._blurMaterial;a!=="latitudinal"&&a!=="longitudinal"&&console.error("blur direction must be either latitudinal or longitudinal!");const u=3,d=new kt(this._lodPlanes[s],c),h=c.uniforms,p=this._sizeLods[n]-1,g=isFinite(r)?Math.PI/(2*p):2*Math.PI/(2*oi-1),_=r/g,m=isFinite(r)?1+Math.floor(u*_):oi;m>oi&&console.warn(`sigmaRadians, ${r}, is too large and will clip, as it requested ${m} samples when the maximum is set to ${oi}`);const f=[];let E=0;for(let C=0;C<oi;++C){const I=C/_,y=Math.exp(-I*I/2);f.push(y),C===0?E+=y:C<m&&(E+=2*y)}for(let C=0;C<f.length;C++)f[C]=f[C]/E;h.envMap.value=e.texture,h.samples.value=m,h.weights.value=f,h.latitudinal.value=a==="latitudinal",o&&(h.poleAxis.value=o);const{_lodMax:b}=this;h.dTheta.value=g,h.mipInt.value=b-n;const v=this._sizeLods[s],T=3*v*(s>b-Ii?s-b+Ii:0),A=4*(this._cubeSize-v);$s(t,T,A,3*v,2*v),l.setRenderTarget(t),l.render(d,ta)}}function Og(i){const e=[],t=[],n=[];let s=i;const r=i-Ii+1+Il.length;for(let a=0;a<r;a++){const o=Math.pow(2,s);t.push(o);let l=1/o;a>i-Ii?l=Il[a-i+Ii-1]:a===0&&(l=0),n.push(l);const c=1/(o-2),u=-c,d=1+c,h=[u,u,d,u,d,d,u,u,d,d,u,d],p=6,g=6,_=3,m=2,f=1,E=new Float32Array(_*g*p),b=new Float32Array(m*g*p),v=new Float32Array(f*g*p);for(let A=0;A<p;A++){const C=A%3*2/3-1,I=A>2?0:-1,y=[C,I,0,C+2/3,I,0,C+2/3,I+1,0,C,I,0,C+2/3,I+1,0,C,I+1,0];E.set(y,_*g*A),b.set(h,m*g*A);const S=[A,A,A,A,A,A];v.set(S,f*g*A)}const T=new jt;T.setAttribute("position",new en(E,_)),T.setAttribute("uv",new en(b,m)),T.setAttribute("faceIndex",new en(v,f)),e.push(T),s>Ii&&s--}return{lodPlanes:e,sizeLods:t,sigmas:n}}function Fl(i,e,t){const n=new di(i,e,t);return n.texture.mapping=_r,n.texture.name="PMREM.cubeUv",n.scissorTest=!0,n}function $s(i,e,t,n,s){i.viewport.set(e,t,n,s),i.scissor.set(e,t,n,s)}function Bg(i,e,t){const n=new Float32Array(oi),s=new O(0,1,0);return new on({name:"SphericalGaussianBlur",defines:{n:oi,CUBEUV_TEXEL_WIDTH:1/e,CUBEUV_TEXEL_HEIGHT:1/t,CUBEUV_MAX_MIP:`${i}.0`},uniforms:{envMap:{value:null},samples:{value:1},weights:{value:n},latitudinal:{value:!1},dTheta:{value:0},mipInt:{value:0},poleAxis:{value:s}},vertexShader:Uo(),fragmentShader:`

			precision mediump float;
			precision mediump int;

			varying vec3 vOutputDirection;

			uniform sampler2D envMap;
			uniform int samples;
			uniform float weights[ n ];
			uniform bool latitudinal;
			uniform float dTheta;
			uniform float mipInt;
			uniform vec3 poleAxis;

			#define ENVMAP_TYPE_CUBE_UV
			#include <cube_uv_reflection_fragment>

			vec3 getSample( float theta, vec3 axis ) {

				float cosTheta = cos( theta );
				// Rodrigues' axis-angle rotation
				vec3 sampleDirection = vOutputDirection * cosTheta
					+ cross( axis, vOutputDirection ) * sin( theta )
					+ axis * dot( axis, vOutputDirection ) * ( 1.0 - cosTheta );

				return bilinearCubeUV( envMap, sampleDirection, mipInt );

			}

			void main() {

				vec3 axis = latitudinal ? poleAxis : cross( poleAxis, vOutputDirection );

				if ( all( equal( axis, vec3( 0.0 ) ) ) ) {

					axis = vec3( vOutputDirection.z, 0.0, - vOutputDirection.x );

				}

				axis = normalize( axis );

				gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
				gl_FragColor.rgb += weights[ 0 ] * getSample( 0.0, axis );

				for ( int i = 1; i < n; i++ ) {

					if ( i >= samples ) {

						break;

					}

					float theta = dTheta * float( i );
					gl_FragColor.rgb += weights[ i ] * getSample( -1.0 * theta, axis );
					gl_FragColor.rgb += weights[ i ] * getSample( theta, axis );

				}

			}
		`,blending:Wn,depthTest:!1,depthWrite:!1})}function Ol(){return new on({name:"EquirectangularToCubeUV",uniforms:{envMap:{value:null}},vertexShader:Uo(),fragmentShader:`

			precision mediump float;
			precision mediump int;

			varying vec3 vOutputDirection;

			uniform sampler2D envMap;

			#include <common>

			void main() {

				vec3 outputDirection = normalize( vOutputDirection );
				vec2 uv = equirectUv( outputDirection );

				gl_FragColor = vec4( texture2D ( envMap, uv ).rgb, 1.0 );

			}
		`,blending:Wn,depthTest:!1,depthWrite:!1})}function Bl(){return new on({name:"CubemapToCubeUV",uniforms:{envMap:{value:null},flipEnvMap:{value:-1}},vertexShader:Uo(),fragmentShader:`

			precision mediump float;
			precision mediump int;

			uniform float flipEnvMap;

			varying vec3 vOutputDirection;

			uniform samplerCube envMap;

			void main() {

				gl_FragColor = textureCube( envMap, vec3( flipEnvMap * vOutputDirection.x, vOutputDirection.yz ) );

			}
		`,blending:Wn,depthTest:!1,depthWrite:!1})}function Uo(){return`

		precision mediump float;
		precision mediump int;

		attribute float faceIndex;

		varying vec3 vOutputDirection;

		// RH coordinate system; PMREM face-indexing convention
		vec3 getDirection( vec2 uv, float face ) {

			uv = 2.0 * uv - 1.0;

			vec3 direction = vec3( uv, 1.0 );

			if ( face == 0.0 ) {

				direction = direction.zyx; // ( 1, v, u ) pos x

			} else if ( face == 1.0 ) {

				direction = direction.xzy;
				direction.xz *= -1.0; // ( -u, 1, -v ) pos y

			} else if ( face == 2.0 ) {

				direction.x *= -1.0; // ( -u, v, 1 ) pos z

			} else if ( face == 3.0 ) {

				direction = direction.zyx;
				direction.xz *= -1.0; // ( -1, v, -u ) neg x

			} else if ( face == 4.0 ) {

				direction = direction.xzy;
				direction.xy *= -1.0; // ( -u, -1, v ) neg y

			} else if ( face == 5.0 ) {

				direction.z *= -1.0; // ( u, v, -1 ) neg z

			}

			return direction;

		}

		void main() {

			vOutputDirection = getDirection( uv, faceIndex );
			gl_Position = vec4( position, 1.0 );

		}
	`}function zg(i){let e=new WeakMap,t=null;function n(o){if(o&&o.isTexture){const l=o.mapping,c=l===Aa||l===Ra,u=l===zi||l===Vi;if(c||u){let d=e.get(o);const h=d!==void 0?d.texture.pmremVersion:0;if(o.isRenderTargetTexture&&o.pmremVersion!==h)return t===null&&(t=new Nl(i)),d=c?t.fromEquirectangular(o,d):t.fromCubemap(o,d),d.texture.pmremVersion=o.pmremVersion,e.set(o,d),d.texture;if(d!==void 0)return d.texture;{const p=o.image;return c&&p&&p.height>0||u&&p&&s(p)?(t===null&&(t=new Nl(i)),d=c?t.fromEquirectangular(o):t.fromCubemap(o),d.texture.pmremVersion=o.pmremVersion,e.set(o,d),o.addEventListener("dispose",r),d.texture):null}}}return o}function s(o){let l=0;const c=6;for(let u=0;u<c;u++)o[u]!==void 0&&l++;return l===c}function r(o){const l=o.target;l.removeEventListener("dispose",r);const c=e.get(l);c!==void 0&&(e.delete(l),c.dispose())}function a(){e=new WeakMap,t!==null&&(t.dispose(),t=null)}return{get:n,dispose:a}}function Vg(i){const e={};function t(n){if(e[n]!==void 0)return e[n];let s;switch(n){case"WEBGL_depth_texture":s=i.getExtension("WEBGL_depth_texture")||i.getExtension("MOZ_WEBGL_depth_texture")||i.getExtension("WEBKIT_WEBGL_depth_texture");break;case"EXT_texture_filter_anisotropic":s=i.getExtension("EXT_texture_filter_anisotropic")||i.getExtension("MOZ_EXT_texture_filter_anisotropic")||i.getExtension("WEBKIT_EXT_texture_filter_anisotropic");break;case"WEBGL_compressed_texture_s3tc":s=i.getExtension("WEBGL_compressed_texture_s3tc")||i.getExtension("MOZ_WEBGL_compressed_texture_s3tc")||i.getExtension("WEBKIT_WEBGL_compressed_texture_s3tc");break;case"WEBGL_compressed_texture_pvrtc":s=i.getExtension("WEBGL_compressed_texture_pvrtc")||i.getExtension("WEBKIT_WEBGL_compressed_texture_pvrtc");break;default:s=i.getExtension(n)}return e[n]=s,s}return{has:function(n){return t(n)!==null},init:function(){t("EXT_color_buffer_float"),t("WEBGL_clip_cull_distance"),t("OES_texture_float_linear"),t("EXT_color_buffer_half_float"),t("WEBGL_multisampled_render_to_texture"),t("WEBGL_render_shared_exponent")},get:function(n){const s=t(n);return s===null&&Ni("THREE.WebGLRenderer: "+n+" extension not supported."),s}}}function Hg(i,e,t,n){const s={},r=new WeakMap;function a(d){const h=d.target;h.index!==null&&e.remove(h.index);for(const g in h.attributes)e.remove(h.attributes[g]);h.removeEventListener("dispose",a),delete s[h.id];const p=r.get(h);p&&(e.remove(p),r.delete(h)),n.releaseStatesOfGeometry(h),h.isInstancedBufferGeometry===!0&&delete h._maxInstanceCount,t.memory.geometries--}function o(d,h){return s[h.id]===!0||(h.addEventListener("dispose",a),s[h.id]=!0,t.memory.geometries++),h}function l(d){const h=d.attributes;for(const p in h)e.update(h[p],i.ARRAY_BUFFER)}function c(d){const h=[],p=d.index,g=d.attributes.position;let _=0;if(p!==null){const E=p.array;_=p.version;for(let b=0,v=E.length;b<v;b+=3){const T=E[b+0],A=E[b+1],C=E[b+2];h.push(T,A,A,C,C,T)}}else if(g!==void 0){const E=g.array;_=g.version;for(let b=0,v=E.length/3-1;b<v;b+=3){const T=b+0,A=b+1,C=b+2;h.push(T,A,A,C,C,T)}}else return;const m=new(Lc(h)?kc:Uc)(h,1);m.version=_;const f=r.get(d);f&&e.remove(f),r.set(d,m)}function u(d){const h=r.get(d);if(h){const p=d.index;p!==null&&h.version<p.version&&c(d)}else c(d);return r.get(d)}return{get:o,update:l,getWireframeAttribute:u}}function Gg(i,e,t){let n;function s(h){n=h}let r,a;function o(h){r=h.type,a=h.bytesPerElement}function l(h,p){i.drawElements(n,p,r,h*a),t.update(p,n,1)}function c(h,p,g){g!==0&&(i.drawElementsInstanced(n,p,r,h*a,g),t.update(p,n,g))}function u(h,p,g){if(g===0)return;e.get("WEBGL_multi_draw").multiDrawElementsWEBGL(n,p,0,r,h,0,g);let m=0;for(let f=0;f<g;f++)m+=p[f];t.update(m,n,1)}function d(h,p,g,_){if(g===0)return;const m=e.get("WEBGL_multi_draw");if(m===null)for(let f=0;f<h.length;f++)c(h[f]/a,p[f],_[f]);else{m.multiDrawElementsInstancedWEBGL(n,p,0,r,h,0,_,0,g);let f=0;for(let E=0;E<g;E++)f+=p[E]*_[E];t.update(f,n,1)}}this.setMode=s,this.setIndex=o,this.render=l,this.renderInstances=c,this.renderMultiDraw=u,this.renderMultiDrawInstances=d}function Wg(i){const e={geometries:0,textures:0},t={frame:0,calls:0,triangles:0,points:0,lines:0};function n(r,a,o){switch(t.calls++,a){case i.TRIANGLES:t.triangles+=o*(r/3);break;case i.LINES:t.lines+=o*(r/2);break;case i.LINE_STRIP:t.lines+=o*(r-1);break;case i.LINE_LOOP:t.lines+=o*r;break;case i.POINTS:t.points+=o*r;break;default:console.error("THREE.WebGLInfo: Unknown draw mode:",a);break}}function s(){t.calls=0,t.triangles=0,t.points=0,t.lines=0}return{memory:e,render:t,programs:null,autoReset:!0,reset:s,update:n}}function $g(i,e,t){const n=new WeakMap,s=new At;function r(a,o,l){const c=a.morphTargetInfluences,u=o.morphAttributes.position||o.morphAttributes.normal||o.morphAttributes.color,d=u!==void 0?u.length:0;let h=n.get(o);if(h===void 0||h.count!==d){let y=function(){C.dispose(),n.delete(o),o.removeEventListener("dispose",y)};h!==void 0&&h.texture.dispose();const p=o.morphAttributes.position!==void 0,g=o.morphAttributes.normal!==void 0,_=o.morphAttributes.color!==void 0,m=o.morphAttributes.position||[],f=o.morphAttributes.normal||[],E=o.morphAttributes.color||[];let b=0;p===!0&&(b=1),g===!0&&(b=2),_===!0&&(b=3);let v=o.attributes.position.count*b,T=1;v>e.maxTextureSize&&(T=Math.ceil(v/e.maxTextureSize),v=e.maxTextureSize);const A=new Float32Array(v*T*4*d),C=new Dc(A,v,T,d);C.type=Pn,C.needsUpdate=!0;const I=b*4;for(let S=0;S<d;S++){const P=m[S],$=f[S],q=E[S],Y=v*T*4*S;for(let X=0;X<P.count;X++){const B=X*I;p===!0&&(s.fromBufferAttribute(P,X),A[Y+B+0]=s.x,A[Y+B+1]=s.y,A[Y+B+2]=s.z,A[Y+B+3]=0),g===!0&&(s.fromBufferAttribute($,X),A[Y+B+4]=s.x,A[Y+B+5]=s.y,A[Y+B+6]=s.z,A[Y+B+7]=0),_===!0&&(s.fromBufferAttribute(q,X),A[Y+B+8]=s.x,A[Y+B+9]=s.y,A[Y+B+10]=s.z,A[Y+B+11]=q.itemSize===4?s.w:1)}}h={count:d,texture:C,size:new et(v,T)},n.set(o,h),o.addEventListener("dispose",y)}if(a.isInstancedMesh===!0&&a.morphTexture!==null)l.getUniforms().setValue(i,"morphTexture",a.morphTexture,t);else{let p=0;for(let _=0;_<c.length;_++)p+=c[_];const g=o.morphTargetsRelative?1:1-p;l.getUniforms().setValue(i,"morphTargetBaseInfluence",g),l.getUniforms().setValue(i,"morphTargetInfluences",c)}l.getUniforms().setValue(i,"morphTargetsTexture",h.texture,t),l.getUniforms().setValue(i,"morphTargetsTextureSize",h.size)}return{update:r}}function Xg(i,e,t,n){let s=new WeakMap;function r(l){const c=n.render.frame,u=l.geometry,d=e.get(l,u);if(s.get(d)!==c&&(e.update(d),s.set(d,c)),l.isInstancedMesh&&(l.hasEventListener("dispose",o)===!1&&l.addEventListener("dispose",o),s.get(l)!==c&&(t.update(l.instanceMatrix,i.ARRAY_BUFFER),l.instanceColor!==null&&t.update(l.instanceColor,i.ARRAY_BUFFER),s.set(l,c))),l.isSkinnedMesh){const h=l.skeleton;s.get(h)!==c&&(h.update(),s.set(h,c))}return d}function a(){s=new WeakMap}function o(l){const c=l.target;c.removeEventListener("dispose",o),t.remove(c.instanceMatrix),c.instanceColor!==null&&t.remove(c.instanceColor)}return{update:r,dispose:a}}const $c=new Bt,zl=new Vc(1,1),Xc=new Dc,qc=new ff,Yc=new Oc,Vl=[],Hl=[],Gl=new Float32Array(16),Wl=new Float32Array(9),$l=new Float32Array(4);function Xi(i,e,t){const n=i[0];if(n<=0||n>0)return i;const s=e*t;let r=Vl[s];if(r===void 0&&(r=new Float32Array(s),Vl[s]=r),e!==0){n.toArray(r,0);for(let a=1,o=0;a!==e;++a)o+=t,i[a].toArray(r,o)}return r}function Pt(i,e){if(i.length!==e.length)return!1;for(let t=0,n=i.length;t<n;t++)if(i[t]!==e[t])return!1;return!0}function Lt(i,e){for(let t=0,n=e.length;t<n;t++)i[t]=e[t]}function Sr(i,e){let t=Hl[e];t===void 0&&(t=new Int32Array(e),Hl[e]=t);for(let n=0;n!==e;++n)t[n]=i.allocateTextureUnit();return t}function qg(i,e){const t=this.cache;t[0]!==e&&(i.uniform1f(this.addr,e),t[0]=e)}function Yg(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y)&&(i.uniform2f(this.addr,e.x,e.y),t[0]=e.x,t[1]=e.y);else{if(Pt(t,e))return;i.uniform2fv(this.addr,e),Lt(t,e)}}function jg(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y||t[2]!==e.z)&&(i.uniform3f(this.addr,e.x,e.y,e.z),t[0]=e.x,t[1]=e.y,t[2]=e.z);else if(e.r!==void 0)(t[0]!==e.r||t[1]!==e.g||t[2]!==e.b)&&(i.uniform3f(this.addr,e.r,e.g,e.b),t[0]=e.r,t[1]=e.g,t[2]=e.b);else{if(Pt(t,e))return;i.uniform3fv(this.addr,e),Lt(t,e)}}function Zg(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y||t[2]!==e.z||t[3]!==e.w)&&(i.uniform4f(this.addr,e.x,e.y,e.z,e.w),t[0]=e.x,t[1]=e.y,t[2]=e.z,t[3]=e.w);else{if(Pt(t,e))return;i.uniform4fv(this.addr,e),Lt(t,e)}}function Kg(i,e){const t=this.cache,n=e.elements;if(n===void 0){if(Pt(t,e))return;i.uniformMatrix2fv(this.addr,!1,e),Lt(t,e)}else{if(Pt(t,n))return;$l.set(n),i.uniformMatrix2fv(this.addr,!1,$l),Lt(t,n)}}function Jg(i,e){const t=this.cache,n=e.elements;if(n===void 0){if(Pt(t,e))return;i.uniformMatrix3fv(this.addr,!1,e),Lt(t,e)}else{if(Pt(t,n))return;Wl.set(n),i.uniformMatrix3fv(this.addr,!1,Wl),Lt(t,n)}}function Qg(i,e){const t=this.cache,n=e.elements;if(n===void 0){if(Pt(t,e))return;i.uniformMatrix4fv(this.addr,!1,e),Lt(t,e)}else{if(Pt(t,n))return;Gl.set(n),i.uniformMatrix4fv(this.addr,!1,Gl),Lt(t,n)}}function e_(i,e){const t=this.cache;t[0]!==e&&(i.uniform1i(this.addr,e),t[0]=e)}function t_(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y)&&(i.uniform2i(this.addr,e.x,e.y),t[0]=e.x,t[1]=e.y);else{if(Pt(t,e))return;i.uniform2iv(this.addr,e),Lt(t,e)}}function n_(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y||t[2]!==e.z)&&(i.uniform3i(this.addr,e.x,e.y,e.z),t[0]=e.x,t[1]=e.y,t[2]=e.z);else{if(Pt(t,e))return;i.uniform3iv(this.addr,e),Lt(t,e)}}function i_(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y||t[2]!==e.z||t[3]!==e.w)&&(i.uniform4i(this.addr,e.x,e.y,e.z,e.w),t[0]=e.x,t[1]=e.y,t[2]=e.z,t[3]=e.w);else{if(Pt(t,e))return;i.uniform4iv(this.addr,e),Lt(t,e)}}function s_(i,e){const t=this.cache;t[0]!==e&&(i.uniform1ui(this.addr,e),t[0]=e)}function r_(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y)&&(i.uniform2ui(this.addr,e.x,e.y),t[0]=e.x,t[1]=e.y);else{if(Pt(t,e))return;i.uniform2uiv(this.addr,e),Lt(t,e)}}function a_(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y||t[2]!==e.z)&&(i.uniform3ui(this.addr,e.x,e.y,e.z),t[0]=e.x,t[1]=e.y,t[2]=e.z);else{if(Pt(t,e))return;i.uniform3uiv(this.addr,e),Lt(t,e)}}function o_(i,e){const t=this.cache;if(e.x!==void 0)(t[0]!==e.x||t[1]!==e.y||t[2]!==e.z||t[3]!==e.w)&&(i.uniform4ui(this.addr,e.x,e.y,e.z,e.w),t[0]=e.x,t[1]=e.y,t[2]=e.z,t[3]=e.w);else{if(Pt(t,e))return;i.uniform4uiv(this.addr,e),Lt(t,e)}}function l_(i,e,t){const n=this.cache,s=t.allocateTextureUnit();n[0]!==s&&(i.uniform1i(this.addr,s),n[0]=s);let r;this.type===i.SAMPLER_2D_SHADOW?(zl.compareFunction=Pc,r=zl):r=$c,t.setTexture2D(e||r,s)}function c_(i,e,t){const n=this.cache,s=t.allocateTextureUnit();n[0]!==s&&(i.uniform1i(this.addr,s),n[0]=s),t.setTexture3D(e||qc,s)}function u_(i,e,t){const n=this.cache,s=t.allocateTextureUnit();n[0]!==s&&(i.uniform1i(this.addr,s),n[0]=s),t.setTextureCube(e||Yc,s)}function d_(i,e,t){const n=this.cache,s=t.allocateTextureUnit();n[0]!==s&&(i.uniform1i(this.addr,s),n[0]=s),t.setTexture2DArray(e||Xc,s)}function h_(i){switch(i){case 5126:return qg;case 35664:return Yg;case 35665:return jg;case 35666:return Zg;case 35674:return Kg;case 35675:return Jg;case 35676:return Qg;case 5124:case 35670:return e_;case 35667:case 35671:return t_;case 35668:case 35672:return n_;case 35669:case 35673:return i_;case 5125:return s_;case 36294:return r_;case 36295:return a_;case 36296:return o_;case 35678:case 36198:case 36298:case 36306:case 35682:return l_;case 35679:case 36299:case 36307:return c_;case 35680:case 36300:case 36308:case 36293:return u_;case 36289:case 36303:case 36311:case 36292:return d_}}function f_(i,e){i.uniform1fv(this.addr,e)}function p_(i,e){const t=Xi(e,this.size,2);i.uniform2fv(this.addr,t)}function m_(i,e){const t=Xi(e,this.size,3);i.uniform3fv(this.addr,t)}function g_(i,e){const t=Xi(e,this.size,4);i.uniform4fv(this.addr,t)}function __(i,e){const t=Xi(e,this.size,4);i.uniformMatrix2fv(this.addr,!1,t)}function v_(i,e){const t=Xi(e,this.size,9);i.uniformMatrix3fv(this.addr,!1,t)}function x_(i,e){const t=Xi(e,this.size,16);i.uniformMatrix4fv(this.addr,!1,t)}function M_(i,e){i.uniform1iv(this.addr,e)}function S_(i,e){i.uniform2iv(this.addr,e)}function y_(i,e){i.uniform3iv(this.addr,e)}function b_(i,e){i.uniform4iv(this.addr,e)}function E_(i,e){i.uniform1uiv(this.addr,e)}function T_(i,e){i.uniform2uiv(this.addr,e)}function w_(i,e){i.uniform3uiv(this.addr,e)}function A_(i,e){i.uniform4uiv(this.addr,e)}function R_(i,e,t){const n=this.cache,s=e.length,r=Sr(t,s);Pt(n,r)||(i.uniform1iv(this.addr,r),Lt(n,r));for(let a=0;a!==s;++a)t.setTexture2D(e[a]||$c,r[a])}function C_(i,e,t){const n=this.cache,s=e.length,r=Sr(t,s);Pt(n,r)||(i.uniform1iv(this.addr,r),Lt(n,r));for(let a=0;a!==s;++a)t.setTexture3D(e[a]||qc,r[a])}function P_(i,e,t){const n=this.cache,s=e.length,r=Sr(t,s);Pt(n,r)||(i.uniform1iv(this.addr,r),Lt(n,r));for(let a=0;a!==s;++a)t.setTextureCube(e[a]||Yc,r[a])}function L_(i,e,t){const n=this.cache,s=e.length,r=Sr(t,s);Pt(n,r)||(i.uniform1iv(this.addr,r),Lt(n,r));for(let a=0;a!==s;++a)t.setTexture2DArray(e[a]||Xc,r[a])}function D_(i){switch(i){case 5126:return f_;case 35664:return p_;case 35665:return m_;case 35666:return g_;case 35674:return __;case 35675:return v_;case 35676:return x_;case 5124:case 35670:return M_;case 35667:case 35671:return S_;case 35668:case 35672:return y_;case 35669:case 35673:return b_;case 5125:return E_;case 36294:return T_;case 36295:return w_;case 36296:return A_;case 35678:case 36198:case 36298:case 36306:case 35682:return R_;case 35679:case 36299:case 36307:return C_;case 35680:case 36300:case 36308:case 36293:return P_;case 36289:case 36303:case 36311:case 36292:return L_}}class I_{constructor(e,t,n){this.id=e,this.addr=n,this.cache=[],this.type=t.type,this.setValue=h_(t.type)}}class U_{constructor(e,t,n){this.id=e,this.addr=n,this.cache=[],this.type=t.type,this.size=t.size,this.setValue=D_(t.type)}}class k_{constructor(e){this.id=e,this.seq=[],this.map={}}setValue(e,t,n){const s=this.seq;for(let r=0,a=s.length;r!==a;++r){const o=s[r];o.setValue(e,t[o.id],n)}}}const aa=/(\w+)(\])?(\[|\.)?/g;function Xl(i,e){i.seq.push(e),i.map[e.id]=e}function N_(i,e,t){const n=i.name,s=n.length;for(aa.lastIndex=0;;){const r=aa.exec(n),a=aa.lastIndex;let o=r[1];const l=r[2]==="]",c=r[3];if(l&&(o=o|0),c===void 0||c==="["&&a+2===s){Xl(t,c===void 0?new I_(o,i,e):new U_(o,i,e));break}else{let d=t.map[o];d===void 0&&(d=new k_(o),Xl(t,d)),t=d}}}class tr{constructor(e,t){this.seq=[],this.map={};const n=e.getProgramParameter(t,e.ACTIVE_UNIFORMS);for(let s=0;s<n;++s){const r=e.getActiveUniform(t,s),a=e.getUniformLocation(t,r.name);N_(r,a,this)}}setValue(e,t,n,s){const r=this.map[t];r!==void 0&&r.setValue(e,n,s)}setOptional(e,t,n){const s=t[n];s!==void 0&&this.setValue(e,n,s)}static upload(e,t,n,s){for(let r=0,a=t.length;r!==a;++r){const o=t[r],l=n[o.id];l.needsUpdate!==!1&&o.setValue(e,l.value,s)}}static seqWithValue(e,t){const n=[];for(let s=0,r=e.length;s!==r;++s){const a=e[s];a.id in t&&n.push(a)}return n}}function ql(i,e,t){const n=i.createShader(e);return i.shaderSource(n,t),i.compileShader(n),n}const F_=37297;let O_=0;function B_(i,e){const t=i.split(`
`),n=[],s=Math.max(e-6,0),r=Math.min(e+6,t.length);for(let a=s;a<r;a++){const o=a+1;n.push(`${o===e?">":" "} ${o}: ${t[a]}`)}return n.join(`
`)}const Yl=new Ze;function z_(i){lt._getMatrix(Yl,lt.workingColorSpace,i);const e=`mat3( ${Yl.elements.map(t=>t.toFixed(4))} )`;switch(lt.getTransfer(i)){case rr:return[e,"LinearTransferOETF"];case mt:return[e,"sRGBTransferOETF"];default:return console.warn("THREE.WebGLProgram: Unsupported color space: ",i),[e,"LinearTransferOETF"]}}function jl(i,e,t){const n=i.getShaderParameter(e,i.COMPILE_STATUS),r=(i.getShaderInfoLog(e)||"").trim();if(n&&r==="")return"";const a=/ERROR: 0:(\d+)/.exec(r);if(a){const o=parseInt(a[1]);return t.toUpperCase()+`

`+r+`

`+B_(i.getShaderSource(e),o)}else return r}function V_(i,e){const t=z_(e);return[`vec4 ${i}( vec4 value ) {`,`	return ${t[1]}( vec4( value.rgb * ${t[0]}, value.a ) );`,"}"].join(`
`)}function H_(i,e){let t;switch(e){case Th:t="Linear";break;case wh:t="Reinhard";break;case Ah:t="Cineon";break;case Rh:t="ACESFilmic";break;case Ph:t="AgX";break;case Lh:t="Neutral";break;case Ch:t="Custom";break;default:console.warn("THREE.WebGLProgram: Unsupported toneMapping:",e),t="Linear"}return"vec3 "+i+"( vec3 color ) { return "+t+"ToneMapping( color ); }"}const Xs=new O;function G_(){lt.getLuminanceCoefficients(Xs);const i=Xs.x.toFixed(4),e=Xs.y.toFixed(4),t=Xs.z.toFixed(4);return["float luminance( const in vec3 rgb ) {",`	const vec3 weights = vec3( ${i}, ${e}, ${t} );`,"	return dot( weights, rgb );","}"].join(`
`)}function W_(i){return[i.extensionClipCullDistance?"#extension GL_ANGLE_clip_cull_distance : require":"",i.extensionMultiDraw?"#extension GL_ANGLE_multi_draw : require":""].filter(ns).join(`
`)}function $_(i){const e=[];for(const t in i){const n=i[t];n!==!1&&e.push("#define "+t+" "+n)}return e.join(`
`)}function X_(i,e){const t={},n=i.getProgramParameter(e,i.ACTIVE_ATTRIBUTES);for(let s=0;s<n;s++){const r=i.getActiveAttrib(e,s),a=r.name;let o=1;r.type===i.FLOAT_MAT2&&(o=2),r.type===i.FLOAT_MAT3&&(o=3),r.type===i.FLOAT_MAT4&&(o=4),t[a]={type:r.type,location:i.getAttribLocation(e,a),locationSize:o}}return t}function ns(i){return i!==""}function Zl(i,e){const t=e.numSpotLightShadows+e.numSpotLightMaps-e.numSpotLightShadowsWithMaps;return i.replace(/NUM_DIR_LIGHTS/g,e.numDirLights).replace(/NUM_SPOT_LIGHTS/g,e.numSpotLights).replace(/NUM_SPOT_LIGHT_MAPS/g,e.numSpotLightMaps).replace(/NUM_SPOT_LIGHT_COORDS/g,t).replace(/NUM_RECT_AREA_LIGHTS/g,e.numRectAreaLights).replace(/NUM_POINT_LIGHTS/g,e.numPointLights).replace(/NUM_HEMI_LIGHTS/g,e.numHemiLights).replace(/NUM_DIR_LIGHT_SHADOWS/g,e.numDirLightShadows).replace(/NUM_SPOT_LIGHT_SHADOWS_WITH_MAPS/g,e.numSpotLightShadowsWithMaps).replace(/NUM_SPOT_LIGHT_SHADOWS/g,e.numSpotLightShadows).replace(/NUM_POINT_LIGHT_SHADOWS/g,e.numPointLightShadows)}function Kl(i,e){return i.replace(/NUM_CLIPPING_PLANES/g,e.numClippingPlanes).replace(/UNION_CLIPPING_PLANES/g,e.numClippingPlanes-e.numClipIntersection)}const q_=/^[ \t]*#include +<([\w\d./]+)>/gm;function ro(i){return i.replace(q_,j_)}const Y_=new Map;function j_(i,e){let t=Ke[e];if(t===void 0){const n=Y_.get(e);if(n!==void 0)t=Ke[n],console.warn('THREE.WebGLRenderer: Shader chunk "%s" has been deprecated. Use "%s" instead.',e,n);else throw new Error("Can not resolve #include <"+e+">")}return ro(t)}const Z_=/#pragma unroll_loop_start\s+for\s*\(\s*int\s+i\s*=\s*(\d+)\s*;\s*i\s*<\s*(\d+)\s*;\s*i\s*\+\+\s*\)\s*{([\s\S]+?)}\s+#pragma unroll_loop_end/g;function Jl(i){return i.replace(Z_,K_)}function K_(i,e,t,n){let s="";for(let r=parseInt(e);r<parseInt(t);r++)s+=n.replace(/\[\s*i\s*\]/g,"[ "+r+" ]").replace(/UNROLLED_LOOP_INDEX/g,r);return s}function Ql(i){let e=`precision ${i.precision} float;
	precision ${i.precision} int;
	precision ${i.precision} sampler2D;
	precision ${i.precision} samplerCube;
	precision ${i.precision} sampler3D;
	precision ${i.precision} sampler2DArray;
	precision ${i.precision} sampler2DShadow;
	precision ${i.precision} samplerCubeShadow;
	precision ${i.precision} sampler2DArrayShadow;
	precision ${i.precision} isampler2D;
	precision ${i.precision} isampler3D;
	precision ${i.precision} isamplerCube;
	precision ${i.precision} isampler2DArray;
	precision ${i.precision} usampler2D;
	precision ${i.precision} usampler3D;
	precision ${i.precision} usamplerCube;
	precision ${i.precision} usampler2DArray;
	`;return i.precision==="highp"?e+=`
#define HIGH_PRECISION`:i.precision==="mediump"?e+=`
#define MEDIUM_PRECISION`:i.precision==="lowp"&&(e+=`
#define LOW_PRECISION`),e}function J_(i){let e="SHADOWMAP_TYPE_BASIC";return i.shadowMapType===xc?e="SHADOWMAP_TYPE_PCF":i.shadowMapType===_o?e="SHADOWMAP_TYPE_PCF_SOFT":i.shadowMapType===Rn&&(e="SHADOWMAP_TYPE_VSM"),e}function Q_(i){let e="ENVMAP_TYPE_CUBE";if(i.envMap)switch(i.envMapMode){case zi:case Vi:e="ENVMAP_TYPE_CUBE";break;case _r:e="ENVMAP_TYPE_CUBE_UV";break}return e}function ev(i){let e="ENVMAP_MODE_REFLECTION";if(i.envMap)switch(i.envMapMode){case Vi:e="ENVMAP_MODE_REFRACTION";break}return e}function tv(i){let e="ENVMAP_BLENDING_NONE";if(i.envMap)switch(i.combine){case vo:e="ENVMAP_BLENDING_MULTIPLY";break;case bh:e="ENVMAP_BLENDING_MIX";break;case Eh:e="ENVMAP_BLENDING_ADD";break}return e}function nv(i){const e=i.envMapCubeUVHeight;if(e===null)return null;const t=Math.log2(e)-2,n=1/e;return{texelWidth:1/(3*Math.max(Math.pow(2,t),7*16)),texelHeight:n,maxMip:t}}function iv(i,e,t,n){const s=i.getContext(),r=t.defines;let a=t.vertexShader,o=t.fragmentShader;const l=J_(t),c=Q_(t),u=ev(t),d=tv(t),h=nv(t),p=W_(t),g=$_(r),_=s.createProgram();let m,f,E=t.glslVersion?"#version "+t.glslVersion+`
`:"";t.isRawShaderMaterial?(m=["#define SHADER_TYPE "+t.shaderType,"#define SHADER_NAME "+t.shaderName,g].filter(ns).join(`
`),m.length>0&&(m+=`
`),f=["#define SHADER_TYPE "+t.shaderType,"#define SHADER_NAME "+t.shaderName,g].filter(ns).join(`
`),f.length>0&&(f+=`
`)):(m=[Ql(t),"#define SHADER_TYPE "+t.shaderType,"#define SHADER_NAME "+t.shaderName,g,t.extensionClipCullDistance?"#define USE_CLIP_DISTANCE":"",t.batching?"#define USE_BATCHING":"",t.batchingColor?"#define USE_BATCHING_COLOR":"",t.instancing?"#define USE_INSTANCING":"",t.instancingColor?"#define USE_INSTANCING_COLOR":"",t.instancingMorph?"#define USE_INSTANCING_MORPH":"",t.useFog&&t.fog?"#define USE_FOG":"",t.useFog&&t.fogExp2?"#define FOG_EXP2":"",t.map?"#define USE_MAP":"",t.envMap?"#define USE_ENVMAP":"",t.envMap?"#define "+u:"",t.lightMap?"#define USE_LIGHTMAP":"",t.aoMap?"#define USE_AOMAP":"",t.bumpMap?"#define USE_BUMPMAP":"",t.normalMap?"#define USE_NORMALMAP":"",t.normalMapObjectSpace?"#define USE_NORMALMAP_OBJECTSPACE":"",t.normalMapTangentSpace?"#define USE_NORMALMAP_TANGENTSPACE":"",t.displacementMap?"#define USE_DISPLACEMENTMAP":"",t.emissiveMap?"#define USE_EMISSIVEMAP":"",t.anisotropy?"#define USE_ANISOTROPY":"",t.anisotropyMap?"#define USE_ANISOTROPYMAP":"",t.clearcoatMap?"#define USE_CLEARCOATMAP":"",t.clearcoatRoughnessMap?"#define USE_CLEARCOAT_ROUGHNESSMAP":"",t.clearcoatNormalMap?"#define USE_CLEARCOAT_NORMALMAP":"",t.iridescenceMap?"#define USE_IRIDESCENCEMAP":"",t.iridescenceThicknessMap?"#define USE_IRIDESCENCE_THICKNESSMAP":"",t.specularMap?"#define USE_SPECULARMAP":"",t.specularColorMap?"#define USE_SPECULAR_COLORMAP":"",t.specularIntensityMap?"#define USE_SPECULAR_INTENSITYMAP":"",t.roughnessMap?"#define USE_ROUGHNESSMAP":"",t.metalnessMap?"#define USE_METALNESSMAP":"",t.alphaMap?"#define USE_ALPHAMAP":"",t.alphaHash?"#define USE_ALPHAHASH":"",t.transmission?"#define USE_TRANSMISSION":"",t.transmissionMap?"#define USE_TRANSMISSIONMAP":"",t.thicknessMap?"#define USE_THICKNESSMAP":"",t.sheenColorMap?"#define USE_SHEEN_COLORMAP":"",t.sheenRoughnessMap?"#define USE_SHEEN_ROUGHNESSMAP":"",t.mapUv?"#define MAP_UV "+t.mapUv:"",t.alphaMapUv?"#define ALPHAMAP_UV "+t.alphaMapUv:"",t.lightMapUv?"#define LIGHTMAP_UV "+t.lightMapUv:"",t.aoMapUv?"#define AOMAP_UV "+t.aoMapUv:"",t.emissiveMapUv?"#define EMISSIVEMAP_UV "+t.emissiveMapUv:"",t.bumpMapUv?"#define BUMPMAP_UV "+t.bumpMapUv:"",t.normalMapUv?"#define NORMALMAP_UV "+t.normalMapUv:"",t.displacementMapUv?"#define DISPLACEMENTMAP_UV "+t.displacementMapUv:"",t.metalnessMapUv?"#define METALNESSMAP_UV "+t.metalnessMapUv:"",t.roughnessMapUv?"#define ROUGHNESSMAP_UV "+t.roughnessMapUv:"",t.anisotropyMapUv?"#define ANISOTROPYMAP_UV "+t.anisotropyMapUv:"",t.clearcoatMapUv?"#define CLEARCOATMAP_UV "+t.clearcoatMapUv:"",t.clearcoatNormalMapUv?"#define CLEARCOAT_NORMALMAP_UV "+t.clearcoatNormalMapUv:"",t.clearcoatRoughnessMapUv?"#define CLEARCOAT_ROUGHNESSMAP_UV "+t.clearcoatRoughnessMapUv:"",t.iridescenceMapUv?"#define IRIDESCENCEMAP_UV "+t.iridescenceMapUv:"",t.iridescenceThicknessMapUv?"#define IRIDESCENCE_THICKNESSMAP_UV "+t.iridescenceThicknessMapUv:"",t.sheenColorMapUv?"#define SHEEN_COLORMAP_UV "+t.sheenColorMapUv:"",t.sheenRoughnessMapUv?"#define SHEEN_ROUGHNESSMAP_UV "+t.sheenRoughnessMapUv:"",t.specularMapUv?"#define SPECULARMAP_UV "+t.specularMapUv:"",t.specularColorMapUv?"#define SPECULAR_COLORMAP_UV "+t.specularColorMapUv:"",t.specularIntensityMapUv?"#define SPECULAR_INTENSITYMAP_UV "+t.specularIntensityMapUv:"",t.transmissionMapUv?"#define TRANSMISSIONMAP_UV "+t.transmissionMapUv:"",t.thicknessMapUv?"#define THICKNESSMAP_UV "+t.thicknessMapUv:"",t.vertexTangents&&t.flatShading===!1?"#define USE_TANGENT":"",t.vertexColors?"#define USE_COLOR":"",t.vertexAlphas?"#define USE_COLOR_ALPHA":"",t.vertexUv1s?"#define USE_UV1":"",t.vertexUv2s?"#define USE_UV2":"",t.vertexUv3s?"#define USE_UV3":"",t.pointsUvs?"#define USE_POINTS_UV":"",t.flatShading?"#define FLAT_SHADED":"",t.skinning?"#define USE_SKINNING":"",t.morphTargets?"#define USE_MORPHTARGETS":"",t.morphNormals&&t.flatShading===!1?"#define USE_MORPHNORMALS":"",t.morphColors?"#define USE_MORPHCOLORS":"",t.morphTargetsCount>0?"#define MORPHTARGETS_TEXTURE_STRIDE "+t.morphTextureStride:"",t.morphTargetsCount>0?"#define MORPHTARGETS_COUNT "+t.morphTargetsCount:"",t.doubleSided?"#define DOUBLE_SIDED":"",t.flipSided?"#define FLIP_SIDED":"",t.shadowMapEnabled?"#define USE_SHADOWMAP":"",t.shadowMapEnabled?"#define "+l:"",t.sizeAttenuation?"#define USE_SIZEATTENUATION":"",t.numLightProbes>0?"#define USE_LIGHT_PROBES":"",t.logarithmicDepthBuffer?"#define USE_LOGDEPTHBUF":"",t.reversedDepthBuffer?"#define USE_REVERSEDEPTHBUF":"","uniform mat4 modelMatrix;","uniform mat4 modelViewMatrix;","uniform mat4 projectionMatrix;","uniform mat4 viewMatrix;","uniform mat3 normalMatrix;","uniform vec3 cameraPosition;","uniform bool isOrthographic;","#ifdef USE_INSTANCING","	attribute mat4 instanceMatrix;","#endif","#ifdef USE_INSTANCING_COLOR","	attribute vec3 instanceColor;","#endif","#ifdef USE_INSTANCING_MORPH","	uniform sampler2D morphTexture;","#endif","attribute vec3 position;","attribute vec3 normal;","attribute vec2 uv;","#ifdef USE_UV1","	attribute vec2 uv1;","#endif","#ifdef USE_UV2","	attribute vec2 uv2;","#endif","#ifdef USE_UV3","	attribute vec2 uv3;","#endif","#ifdef USE_TANGENT","	attribute vec4 tangent;","#endif","#if defined( USE_COLOR_ALPHA )","	attribute vec4 color;","#elif defined( USE_COLOR )","	attribute vec3 color;","#endif","#ifdef USE_SKINNING","	attribute vec4 skinIndex;","	attribute vec4 skinWeight;","#endif",`
`].filter(ns).join(`
`),f=[Ql(t),"#define SHADER_TYPE "+t.shaderType,"#define SHADER_NAME "+t.shaderName,g,t.useFog&&t.fog?"#define USE_FOG":"",t.useFog&&t.fogExp2?"#define FOG_EXP2":"",t.alphaToCoverage?"#define ALPHA_TO_COVERAGE":"",t.map?"#define USE_MAP":"",t.matcap?"#define USE_MATCAP":"",t.envMap?"#define USE_ENVMAP":"",t.envMap?"#define "+c:"",t.envMap?"#define "+u:"",t.envMap?"#define "+d:"",h?"#define CUBEUV_TEXEL_WIDTH "+h.texelWidth:"",h?"#define CUBEUV_TEXEL_HEIGHT "+h.texelHeight:"",h?"#define CUBEUV_MAX_MIP "+h.maxMip+".0":"",t.lightMap?"#define USE_LIGHTMAP":"",t.aoMap?"#define USE_AOMAP":"",t.bumpMap?"#define USE_BUMPMAP":"",t.normalMap?"#define USE_NORMALMAP":"",t.normalMapObjectSpace?"#define USE_NORMALMAP_OBJECTSPACE":"",t.normalMapTangentSpace?"#define USE_NORMALMAP_TANGENTSPACE":"",t.emissiveMap?"#define USE_EMISSIVEMAP":"",t.anisotropy?"#define USE_ANISOTROPY":"",t.anisotropyMap?"#define USE_ANISOTROPYMAP":"",t.clearcoat?"#define USE_CLEARCOAT":"",t.clearcoatMap?"#define USE_CLEARCOATMAP":"",t.clearcoatRoughnessMap?"#define USE_CLEARCOAT_ROUGHNESSMAP":"",t.clearcoatNormalMap?"#define USE_CLEARCOAT_NORMALMAP":"",t.dispersion?"#define USE_DISPERSION":"",t.iridescence?"#define USE_IRIDESCENCE":"",t.iridescenceMap?"#define USE_IRIDESCENCEMAP":"",t.iridescenceThicknessMap?"#define USE_IRIDESCENCE_THICKNESSMAP":"",t.specularMap?"#define USE_SPECULARMAP":"",t.specularColorMap?"#define USE_SPECULAR_COLORMAP":"",t.specularIntensityMap?"#define USE_SPECULAR_INTENSITYMAP":"",t.roughnessMap?"#define USE_ROUGHNESSMAP":"",t.metalnessMap?"#define USE_METALNESSMAP":"",t.alphaMap?"#define USE_ALPHAMAP":"",t.alphaTest?"#define USE_ALPHATEST":"",t.alphaHash?"#define USE_ALPHAHASH":"",t.sheen?"#define USE_SHEEN":"",t.sheenColorMap?"#define USE_SHEEN_COLORMAP":"",t.sheenRoughnessMap?"#define USE_SHEEN_ROUGHNESSMAP":"",t.transmission?"#define USE_TRANSMISSION":"",t.transmissionMap?"#define USE_TRANSMISSIONMAP":"",t.thicknessMap?"#define USE_THICKNESSMAP":"",t.vertexTangents&&t.flatShading===!1?"#define USE_TANGENT":"",t.vertexColors||t.instancingColor||t.batchingColor?"#define USE_COLOR":"",t.vertexAlphas?"#define USE_COLOR_ALPHA":"",t.vertexUv1s?"#define USE_UV1":"",t.vertexUv2s?"#define USE_UV2":"",t.vertexUv3s?"#define USE_UV3":"",t.pointsUvs?"#define USE_POINTS_UV":"",t.gradientMap?"#define USE_GRADIENTMAP":"",t.flatShading?"#define FLAT_SHADED":"",t.doubleSided?"#define DOUBLE_SIDED":"",t.flipSided?"#define FLIP_SIDED":"",t.shadowMapEnabled?"#define USE_SHADOWMAP":"",t.shadowMapEnabled?"#define "+l:"",t.premultipliedAlpha?"#define PREMULTIPLIED_ALPHA":"",t.numLightProbes>0?"#define USE_LIGHT_PROBES":"",t.decodeVideoTexture?"#define DECODE_VIDEO_TEXTURE":"",t.decodeVideoTextureEmissive?"#define DECODE_VIDEO_TEXTURE_EMISSIVE":"",t.logarithmicDepthBuffer?"#define USE_LOGDEPTHBUF":"",t.reversedDepthBuffer?"#define USE_REVERSEDEPTHBUF":"","uniform mat4 viewMatrix;","uniform vec3 cameraPosition;","uniform bool isOrthographic;",t.toneMapping!==$n?"#define TONE_MAPPING":"",t.toneMapping!==$n?Ke.tonemapping_pars_fragment:"",t.toneMapping!==$n?H_("toneMapping",t.toneMapping):"",t.dithering?"#define DITHERING":"",t.opaque?"#define OPAQUE":"",Ke.colorspace_pars_fragment,V_("linearToOutputTexel",t.outputColorSpace),G_(),t.useDepthPacking?"#define DEPTH_PACKING "+t.depthPacking:"",`
`].filter(ns).join(`
`)),a=ro(a),a=Zl(a,t),a=Kl(a,t),o=ro(o),o=Zl(o,t),o=Kl(o,t),a=Jl(a),o=Jl(o),t.isRawShaderMaterial!==!0&&(E=`#version 300 es
`,m=[p,"#define attribute in","#define varying out","#define texture2D texture"].join(`
`)+`
`+m,f=["#define varying in",t.glslVersion===rl?"":"layout(location = 0) out highp vec4 pc_fragColor;",t.glslVersion===rl?"":"#define gl_FragColor pc_fragColor","#define gl_FragDepthEXT gl_FragDepth","#define texture2D texture","#define textureCube texture","#define texture2DProj textureProj","#define texture2DLodEXT textureLod","#define texture2DProjLodEXT textureProjLod","#define textureCubeLodEXT textureLod","#define texture2DGradEXT textureGrad","#define texture2DProjGradEXT textureProjGrad","#define textureCubeGradEXT textureGrad"].join(`
`)+`
`+f);const b=E+m+a,v=E+f+o,T=ql(s,s.VERTEX_SHADER,b),A=ql(s,s.FRAGMENT_SHADER,v);s.attachShader(_,T),s.attachShader(_,A),t.index0AttributeName!==void 0?s.bindAttribLocation(_,0,t.index0AttributeName):t.morphTargets===!0&&s.bindAttribLocation(_,0,"position"),s.linkProgram(_);function C(P){if(i.debug.checkShaderErrors){const $=s.getProgramInfoLog(_)||"",q=s.getShaderInfoLog(T)||"",Y=s.getShaderInfoLog(A)||"",X=$.trim(),B=q.trim(),Q=Y.trim();let G=!0,ie=!0;if(s.getProgramParameter(_,s.LINK_STATUS)===!1)if(G=!1,typeof i.debug.onShaderError=="function")i.debug.onShaderError(s,_,T,A);else{const de=jl(s,T,"vertex"),Ee=jl(s,A,"fragment");console.error("THREE.WebGLProgram: Shader Error "+s.getError()+" - VALIDATE_STATUS "+s.getProgramParameter(_,s.VALIDATE_STATUS)+`

Material Name: `+P.name+`
Material Type: `+P.type+`

Program Info Log: `+X+`
`+de+`
`+Ee)}else X!==""?console.warn("THREE.WebGLProgram: Program Info Log:",X):(B===""||Q==="")&&(ie=!1);ie&&(P.diagnostics={runnable:G,programLog:X,vertexShader:{log:B,prefix:m},fragmentShader:{log:Q,prefix:f}})}s.deleteShader(T),s.deleteShader(A),I=new tr(s,_),y=X_(s,_)}let I;this.getUniforms=function(){return I===void 0&&C(this),I};let y;this.getAttributes=function(){return y===void 0&&C(this),y};let S=t.rendererExtensionParallelShaderCompile===!1;return this.isReady=function(){return S===!1&&(S=s.getProgramParameter(_,F_)),S},this.destroy=function(){n.releaseStatesOfProgram(this),s.deleteProgram(_),this.program=void 0},this.type=t.shaderType,this.name=t.shaderName,this.id=O_++,this.cacheKey=e,this.usedTimes=1,this.program=_,this.vertexShader=T,this.fragmentShader=A,this}let sv=0;class rv{constructor(){this.shaderCache=new Map,this.materialCache=new Map}update(e){const t=e.vertexShader,n=e.fragmentShader,s=this._getShaderStage(t),r=this._getShaderStage(n),a=this._getShaderCacheForMaterial(e);return a.has(s)===!1&&(a.add(s),s.usedTimes++),a.has(r)===!1&&(a.add(r),r.usedTimes++),this}remove(e){const t=this.materialCache.get(e);for(const n of t)n.usedTimes--,n.usedTimes===0&&this.shaderCache.delete(n.code);return this.materialCache.delete(e),this}getVertexShaderID(e){return this._getShaderStage(e.vertexShader).id}getFragmentShaderID(e){return this._getShaderStage(e.fragmentShader).id}dispose(){this.shaderCache.clear(),this.materialCache.clear()}_getShaderCacheForMaterial(e){const t=this.materialCache;let n=t.get(e);return n===void 0&&(n=new Set,t.set(e,n)),n}_getShaderStage(e){const t=this.shaderCache;let n=t.get(e);return n===void 0&&(n=new av(e),t.set(e,n)),n}}class av{constructor(e){this.id=sv++,this.code=e,this.usedTimes=0}}function ov(i,e,t,n,s,r,a){const o=new Ao,l=new rv,c=new Set,u=[],d=s.logarithmicDepthBuffer,h=s.vertexTextures;let p=s.precision;const g={MeshDepthMaterial:"depth",MeshDistanceMaterial:"distanceRGBA",MeshNormalMaterial:"normal",MeshBasicMaterial:"basic",MeshLambertMaterial:"lambert",MeshPhongMaterial:"phong",MeshToonMaterial:"toon",MeshStandardMaterial:"physical",MeshPhysicalMaterial:"physical",MeshMatcapMaterial:"matcap",LineBasicMaterial:"basic",LineDashedMaterial:"dashed",PointsMaterial:"points",ShadowMaterial:"shadow",SpriteMaterial:"sprite"};function _(y){return c.add(y),y===0?"uv":`uv${y}`}function m(y,S,P,$,q){const Y=$.fog,X=q.geometry,B=y.isMeshStandardMaterial?$.environment:null,Q=(y.isMeshStandardMaterial?t:e).get(y.envMap||B),G=Q&&Q.mapping===_r?Q.image.height:null,ie=g[y.type];y.precision!==null&&(p=s.getMaxPrecision(y.precision),p!==y.precision&&console.warn("THREE.WebGLProgram.getParameters:",y.precision,"not supported, using",p,"instead."));const de=X.morphAttributes.position||X.morphAttributes.normal||X.morphAttributes.color,Ee=de!==void 0?de.length:0;let Oe=0;X.morphAttributes.position!==void 0&&(Oe=1),X.morphAttributes.normal!==void 0&&(Oe=2),X.morphAttributes.color!==void 0&&(Oe=3);let at,He,j,he;if(ie){const ut=gn[ie];at=ut.vertexShader,He=ut.fragmentShader}else at=y.vertexShader,He=y.fragmentShader,l.update(y),j=l.getVertexShaderID(y),he=l.getFragmentShaderID(y);const le=i.getRenderTarget(),Be=i.state.buffers.depth.getReversed(),Ue=q.isInstancedMesh===!0,$e=q.isBatchedMesh===!0,Mt=!!y.map,st=!!y.matcap,D=!!Q,ft=!!y.aoMap,Ne=!!y.lightMap,rt=!!y.bumpMap,ke=!!y.normalMap,yt=!!y.displacementMap,Se=!!y.emissiveMap,Je=!!y.metalnessMap,Dt=!!y.roughnessMap,Rt=y.anisotropy>0,R=y.clearcoat>0,x=y.dispersion>0,z=y.iridescence>0,K=y.sheen>0,ne=y.transmission>0,Z=Rt&&!!y.anisotropyMap,Pe=R&&!!y.clearcoatMap,ce=R&&!!y.clearcoatNormalMap,we=R&&!!y.clearcoatRoughnessMap,Ae=z&&!!y.iridescenceMap,ae=z&&!!y.iridescenceThicknessMap,ge=K&&!!y.sheenColorMap,Ge=K&&!!y.sheenRoughnessMap,Re=!!y.specularMap,pe=!!y.specularColorMap,je=!!y.specularIntensityMap,U=ne&&!!y.transmissionMap,oe=ne&&!!y.thicknessMap,ue=!!y.gradientMap,Me=!!y.alphaMap,se=y.alphaTest>0,ee=!!y.alphaHash,Te=!!y.extensions;let Ye=$n;y.toneMapped&&(le===null||le.isXRRenderTarget===!0)&&(Ye=i.toneMapping);const xt={shaderID:ie,shaderType:y.type,shaderName:y.name,vertexShader:at,fragmentShader:He,defines:y.defines,customVertexShaderID:j,customFragmentShaderID:he,isRawShaderMaterial:y.isRawShaderMaterial===!0,glslVersion:y.glslVersion,precision:p,batching:$e,batchingColor:$e&&q._colorsTexture!==null,instancing:Ue,instancingColor:Ue&&q.instanceColor!==null,instancingMorph:Ue&&q.morphTexture!==null,supportsVertexTextures:h,outputColorSpace:le===null?i.outputColorSpace:le.isXRRenderTarget===!0?le.texture.colorSpace:Hi,alphaToCoverage:!!y.alphaToCoverage,map:Mt,matcap:st,envMap:D,envMapMode:D&&Q.mapping,envMapCubeUVHeight:G,aoMap:ft,lightMap:Ne,bumpMap:rt,normalMap:ke,displacementMap:h&&yt,emissiveMap:Se,normalMapObjectSpace:ke&&y.normalMapType===kh,normalMapTangentSpace:ke&&y.normalMapType===Cc,metalnessMap:Je,roughnessMap:Dt,anisotropy:Rt,anisotropyMap:Z,clearcoat:R,clearcoatMap:Pe,clearcoatNormalMap:ce,clearcoatRoughnessMap:we,dispersion:x,iridescence:z,iridescenceMap:Ae,iridescenceThicknessMap:ae,sheen:K,sheenColorMap:ge,sheenRoughnessMap:Ge,specularMap:Re,specularColorMap:pe,specularIntensityMap:je,transmission:ne,transmissionMap:U,thicknessMap:oe,gradientMap:ue,opaque:y.transparent===!1&&y.blending===ki&&y.alphaToCoverage===!1,alphaMap:Me,alphaTest:se,alphaHash:ee,combine:y.combine,mapUv:Mt&&_(y.map.channel),aoMapUv:ft&&_(y.aoMap.channel),lightMapUv:Ne&&_(y.lightMap.channel),bumpMapUv:rt&&_(y.bumpMap.channel),normalMapUv:ke&&_(y.normalMap.channel),displacementMapUv:yt&&_(y.displacementMap.channel),emissiveMapUv:Se&&_(y.emissiveMap.channel),metalnessMapUv:Je&&_(y.metalnessMap.channel),roughnessMapUv:Dt&&_(y.roughnessMap.channel),anisotropyMapUv:Z&&_(y.anisotropyMap.channel),clearcoatMapUv:Pe&&_(y.clearcoatMap.channel),clearcoatNormalMapUv:ce&&_(y.clearcoatNormalMap.channel),clearcoatRoughnessMapUv:we&&_(y.clearcoatRoughnessMap.channel),iridescenceMapUv:Ae&&_(y.iridescenceMap.channel),iridescenceThicknessMapUv:ae&&_(y.iridescenceThicknessMap.channel),sheenColorMapUv:ge&&_(y.sheenColorMap.channel),sheenRoughnessMapUv:Ge&&_(y.sheenRoughnessMap.channel),specularMapUv:Re&&_(y.specularMap.channel),specularColorMapUv:pe&&_(y.specularColorMap.channel),specularIntensityMapUv:je&&_(y.specularIntensityMap.channel),transmissionMapUv:U&&_(y.transmissionMap.channel),thicknessMapUv:oe&&_(y.thicknessMap.channel),alphaMapUv:Me&&_(y.alphaMap.channel),vertexTangents:!!X.attributes.tangent&&(ke||Rt),vertexColors:y.vertexColors,vertexAlphas:y.vertexColors===!0&&!!X.attributes.color&&X.attributes.color.itemSize===4,pointsUvs:q.isPoints===!0&&!!X.attributes.uv&&(Mt||Me),fog:!!Y,useFog:y.fog===!0,fogExp2:!!Y&&Y.isFogExp2,flatShading:y.flatShading===!0&&y.wireframe===!1,sizeAttenuation:y.sizeAttenuation===!0,logarithmicDepthBuffer:d,reversedDepthBuffer:Be,skinning:q.isSkinnedMesh===!0,morphTargets:X.morphAttributes.position!==void 0,morphNormals:X.morphAttributes.normal!==void 0,morphColors:X.morphAttributes.color!==void 0,morphTargetsCount:Ee,morphTextureStride:Oe,numDirLights:S.directional.length,numPointLights:S.point.length,numSpotLights:S.spot.length,numSpotLightMaps:S.spotLightMap.length,numRectAreaLights:S.rectArea.length,numHemiLights:S.hemi.length,numDirLightShadows:S.directionalShadowMap.length,numPointLightShadows:S.pointShadowMap.length,numSpotLightShadows:S.spotShadowMap.length,numSpotLightShadowsWithMaps:S.numSpotLightShadowsWithMaps,numLightProbes:S.numLightProbes,numClippingPlanes:a.numPlanes,numClipIntersection:a.numIntersection,dithering:y.dithering,shadowMapEnabled:i.shadowMap.enabled&&P.length>0,shadowMapType:i.shadowMap.type,toneMapping:Ye,decodeVideoTexture:Mt&&y.map.isVideoTexture===!0&&lt.getTransfer(y.map.colorSpace)===mt,decodeVideoTextureEmissive:Se&&y.emissiveMap.isVideoTexture===!0&&lt.getTransfer(y.emissiveMap.colorSpace)===mt,premultipliedAlpha:y.premultipliedAlpha,doubleSided:y.side===qt,flipSided:y.side===Yt,useDepthPacking:y.depthPacking>=0,depthPacking:y.depthPacking||0,index0AttributeName:y.index0AttributeName,extensionClipCullDistance:Te&&y.extensions.clipCullDistance===!0&&n.has("WEBGL_clip_cull_distance"),extensionMultiDraw:(Te&&y.extensions.multiDraw===!0||$e)&&n.has("WEBGL_multi_draw"),rendererExtensionParallelShaderCompile:n.has("KHR_parallel_shader_compile"),customProgramCacheKey:y.customProgramCacheKey()};return xt.vertexUv1s=c.has(1),xt.vertexUv2s=c.has(2),xt.vertexUv3s=c.has(3),c.clear(),xt}function f(y){const S=[];if(y.shaderID?S.push(y.shaderID):(S.push(y.customVertexShaderID),S.push(y.customFragmentShaderID)),y.defines!==void 0)for(const P in y.defines)S.push(P),S.push(y.defines[P]);return y.isRawShaderMaterial===!1&&(E(S,y),b(S,y),S.push(i.outputColorSpace)),S.push(y.customProgramCacheKey),S.join()}function E(y,S){y.push(S.precision),y.push(S.outputColorSpace),y.push(S.envMapMode),y.push(S.envMapCubeUVHeight),y.push(S.mapUv),y.push(S.alphaMapUv),y.push(S.lightMapUv),y.push(S.aoMapUv),y.push(S.bumpMapUv),y.push(S.normalMapUv),y.push(S.displacementMapUv),y.push(S.emissiveMapUv),y.push(S.metalnessMapUv),y.push(S.roughnessMapUv),y.push(S.anisotropyMapUv),y.push(S.clearcoatMapUv),y.push(S.clearcoatNormalMapUv),y.push(S.clearcoatRoughnessMapUv),y.push(S.iridescenceMapUv),y.push(S.iridescenceThicknessMapUv),y.push(S.sheenColorMapUv),y.push(S.sheenRoughnessMapUv),y.push(S.specularMapUv),y.push(S.specularColorMapUv),y.push(S.specularIntensityMapUv),y.push(S.transmissionMapUv),y.push(S.thicknessMapUv),y.push(S.combine),y.push(S.fogExp2),y.push(S.sizeAttenuation),y.push(S.morphTargetsCount),y.push(S.morphAttributeCount),y.push(S.numDirLights),y.push(S.numPointLights),y.push(S.numSpotLights),y.push(S.numSpotLightMaps),y.push(S.numHemiLights),y.push(S.numRectAreaLights),y.push(S.numDirLightShadows),y.push(S.numPointLightShadows),y.push(S.numSpotLightShadows),y.push(S.numSpotLightShadowsWithMaps),y.push(S.numLightProbes),y.push(S.shadowMapType),y.push(S.toneMapping),y.push(S.numClippingPlanes),y.push(S.numClipIntersection),y.push(S.depthPacking)}function b(y,S){o.disableAll(),S.supportsVertexTextures&&o.enable(0),S.instancing&&o.enable(1),S.instancingColor&&o.enable(2),S.instancingMorph&&o.enable(3),S.matcap&&o.enable(4),S.envMap&&o.enable(5),S.normalMapObjectSpace&&o.enable(6),S.normalMapTangentSpace&&o.enable(7),S.clearcoat&&o.enable(8),S.iridescence&&o.enable(9),S.alphaTest&&o.enable(10),S.vertexColors&&o.enable(11),S.vertexAlphas&&o.enable(12),S.vertexUv1s&&o.enable(13),S.vertexUv2s&&o.enable(14),S.vertexUv3s&&o.enable(15),S.vertexTangents&&o.enable(16),S.anisotropy&&o.enable(17),S.alphaHash&&o.enable(18),S.batching&&o.enable(19),S.dispersion&&o.enable(20),S.batchingColor&&o.enable(21),S.gradientMap&&o.enable(22),y.push(o.mask),o.disableAll(),S.fog&&o.enable(0),S.useFog&&o.enable(1),S.flatShading&&o.enable(2),S.logarithmicDepthBuffer&&o.enable(3),S.reversedDepthBuffer&&o.enable(4),S.skinning&&o.enable(5),S.morphTargets&&o.enable(6),S.morphNormals&&o.enable(7),S.morphColors&&o.enable(8),S.premultipliedAlpha&&o.enable(9),S.shadowMapEnabled&&o.enable(10),S.doubleSided&&o.enable(11),S.flipSided&&o.enable(12),S.useDepthPacking&&o.enable(13),S.dithering&&o.enable(14),S.transmission&&o.enable(15),S.sheen&&o.enable(16),S.opaque&&o.enable(17),S.pointsUvs&&o.enable(18),S.decodeVideoTexture&&o.enable(19),S.decodeVideoTextureEmissive&&o.enable(20),S.alphaToCoverage&&o.enable(21),y.push(o.mask)}function v(y){const S=g[y.type];let P;if(S){const $=gn[S];P=wf.clone($.uniforms)}else P=y.uniforms;return P}function T(y,S){let P;for(let $=0,q=u.length;$<q;$++){const Y=u[$];if(Y.cacheKey===S){P=Y,++P.usedTimes;break}}return P===void 0&&(P=new iv(i,S,y,r),u.push(P)),P}function A(y){if(--y.usedTimes===0){const S=u.indexOf(y);u[S]=u[u.length-1],u.pop(),y.destroy()}}function C(y){l.remove(y)}function I(){l.dispose()}return{getParameters:m,getProgramCacheKey:f,getUniforms:v,acquireProgram:T,releaseProgram:A,releaseShaderCache:C,programs:u,dispose:I}}function lv(){let i=new WeakMap;function e(a){return i.has(a)}function t(a){let o=i.get(a);return o===void 0&&(o={},i.set(a,o)),o}function n(a){i.delete(a)}function s(a,o,l){i.get(a)[o]=l}function r(){i=new WeakMap}return{has:e,get:t,remove:n,update:s,dispose:r}}function cv(i,e){return i.groupOrder!==e.groupOrder?i.groupOrder-e.groupOrder:i.renderOrder!==e.renderOrder?i.renderOrder-e.renderOrder:i.material.id!==e.material.id?i.material.id-e.material.id:i.z!==e.z?i.z-e.z:i.id-e.id}function ec(i,e){return i.groupOrder!==e.groupOrder?i.groupOrder-e.groupOrder:i.renderOrder!==e.renderOrder?i.renderOrder-e.renderOrder:i.z!==e.z?e.z-i.z:i.id-e.id}function tc(){const i=[];let e=0;const t=[],n=[],s=[];function r(){e=0,t.length=0,n.length=0,s.length=0}function a(d,h,p,g,_,m){let f=i[e];return f===void 0?(f={id:d.id,object:d,geometry:h,material:p,groupOrder:g,renderOrder:d.renderOrder,z:_,group:m},i[e]=f):(f.id=d.id,f.object=d,f.geometry=h,f.material=p,f.groupOrder=g,f.renderOrder=d.renderOrder,f.z=_,f.group=m),e++,f}function o(d,h,p,g,_,m){const f=a(d,h,p,g,_,m);p.transmission>0?n.push(f):p.transparent===!0?s.push(f):t.push(f)}function l(d,h,p,g,_,m){const f=a(d,h,p,g,_,m);p.transmission>0?n.unshift(f):p.transparent===!0?s.unshift(f):t.unshift(f)}function c(d,h){t.length>1&&t.sort(d||cv),n.length>1&&n.sort(h||ec),s.length>1&&s.sort(h||ec)}function u(){for(let d=e,h=i.length;d<h;d++){const p=i[d];if(p.id===null)break;p.id=null,p.object=null,p.geometry=null,p.material=null,p.group=null}}return{opaque:t,transmissive:n,transparent:s,init:r,push:o,unshift:l,finish:u,sort:c}}function uv(){let i=new WeakMap;function e(n,s){const r=i.get(n);let a;return r===void 0?(a=new tc,i.set(n,[a])):s>=r.length?(a=new tc,r.push(a)):a=r[s],a}function t(){i=new WeakMap}return{get:e,dispose:t}}function dv(){const i={};return{get:function(e){if(i[e.id]!==void 0)return i[e.id];let t;switch(e.type){case"DirectionalLight":t={direction:new O,color:new Le};break;case"SpotLight":t={position:new O,direction:new O,color:new Le,distance:0,coneCos:0,penumbraCos:0,decay:0};break;case"PointLight":t={position:new O,color:new Le,distance:0,decay:0};break;case"HemisphereLight":t={direction:new O,skyColor:new Le,groundColor:new Le};break;case"RectAreaLight":t={color:new Le,position:new O,halfWidth:new O,halfHeight:new O};break}return i[e.id]=t,t}}}function hv(){const i={};return{get:function(e){if(i[e.id]!==void 0)return i[e.id];let t;switch(e.type){case"DirectionalLight":t={shadowIntensity:1,shadowBias:0,shadowNormalBias:0,shadowRadius:1,shadowMapSize:new et};break;case"SpotLight":t={shadowIntensity:1,shadowBias:0,shadowNormalBias:0,shadowRadius:1,shadowMapSize:new et};break;case"PointLight":t={shadowIntensity:1,shadowBias:0,shadowNormalBias:0,shadowRadius:1,shadowMapSize:new et,shadowCameraNear:1,shadowCameraFar:1e3};break}return i[e.id]=t,t}}}let fv=0;function pv(i,e){return(e.castShadow?2:0)-(i.castShadow?2:0)+(e.map?1:0)-(i.map?1:0)}function mv(i){const e=new dv,t=hv(),n={version:0,hash:{directionalLength:-1,pointLength:-1,spotLength:-1,rectAreaLength:-1,hemiLength:-1,numDirectionalShadows:-1,numPointShadows:-1,numSpotShadows:-1,numSpotMaps:-1,numLightProbes:-1},ambient:[0,0,0],probe:[],directional:[],directionalShadow:[],directionalShadowMap:[],directionalShadowMatrix:[],spot:[],spotLightMap:[],spotShadow:[],spotShadowMap:[],spotLightMatrix:[],rectArea:[],rectAreaLTC1:null,rectAreaLTC2:null,point:[],pointShadow:[],pointShadowMap:[],pointShadowMatrix:[],hemi:[],numSpotLightShadowsWithMaps:0,numLightProbes:0};for(let c=0;c<9;c++)n.probe.push(new O);const s=new O,r=new bt,a=new bt;function o(c){let u=0,d=0,h=0;for(let y=0;y<9;y++)n.probe[y].set(0,0,0);let p=0,g=0,_=0,m=0,f=0,E=0,b=0,v=0,T=0,A=0,C=0;c.sort(pv);for(let y=0,S=c.length;y<S;y++){const P=c[y],$=P.color,q=P.intensity,Y=P.distance,X=P.shadow&&P.shadow.map?P.shadow.map.texture:null;if(P.isAmbientLight)u+=$.r*q,d+=$.g*q,h+=$.b*q;else if(P.isLightProbe){for(let B=0;B<9;B++)n.probe[B].addScaledVector(P.sh.coefficients[B],q);C++}else if(P.isDirectionalLight){const B=e.get(P);if(B.color.copy(P.color).multiplyScalar(P.intensity),P.castShadow){const Q=P.shadow,G=t.get(P);G.shadowIntensity=Q.intensity,G.shadowBias=Q.bias,G.shadowNormalBias=Q.normalBias,G.shadowRadius=Q.radius,G.shadowMapSize=Q.mapSize,n.directionalShadow[p]=G,n.directionalShadowMap[p]=X,n.directionalShadowMatrix[p]=P.shadow.matrix,E++}n.directional[p]=B,p++}else if(P.isSpotLight){const B=e.get(P);B.position.setFromMatrixPosition(P.matrixWorld),B.color.copy($).multiplyScalar(q),B.distance=Y,B.coneCos=Math.cos(P.angle),B.penumbraCos=Math.cos(P.angle*(1-P.penumbra)),B.decay=P.decay,n.spot[_]=B;const Q=P.shadow;if(P.map&&(n.spotLightMap[T]=P.map,T++,Q.updateMatrices(P),P.castShadow&&A++),n.spotLightMatrix[_]=Q.matrix,P.castShadow){const G=t.get(P);G.shadowIntensity=Q.intensity,G.shadowBias=Q.bias,G.shadowNormalBias=Q.normalBias,G.shadowRadius=Q.radius,G.shadowMapSize=Q.mapSize,n.spotShadow[_]=G,n.spotShadowMap[_]=X,v++}_++}else if(P.isRectAreaLight){const B=e.get(P);B.color.copy($).multiplyScalar(q),B.halfWidth.set(P.width*.5,0,0),B.halfHeight.set(0,P.height*.5,0),n.rectArea[m]=B,m++}else if(P.isPointLight){const B=e.get(P);if(B.color.copy(P.color).multiplyScalar(P.intensity),B.distance=P.distance,B.decay=P.decay,P.castShadow){const Q=P.shadow,G=t.get(P);G.shadowIntensity=Q.intensity,G.shadowBias=Q.bias,G.shadowNormalBias=Q.normalBias,G.shadowRadius=Q.radius,G.shadowMapSize=Q.mapSize,G.shadowCameraNear=Q.camera.near,G.shadowCameraFar=Q.camera.far,n.pointShadow[g]=G,n.pointShadowMap[g]=X,n.pointShadowMatrix[g]=P.shadow.matrix,b++}n.point[g]=B,g++}else if(P.isHemisphereLight){const B=e.get(P);B.skyColor.copy(P.color).multiplyScalar(q),B.groundColor.copy(P.groundColor).multiplyScalar(q),n.hemi[f]=B,f++}}m>0&&(i.has("OES_texture_float_linear")===!0?(n.rectAreaLTC1=fe.LTC_FLOAT_1,n.rectAreaLTC2=fe.LTC_FLOAT_2):(n.rectAreaLTC1=fe.LTC_HALF_1,n.rectAreaLTC2=fe.LTC_HALF_2)),n.ambient[0]=u,n.ambient[1]=d,n.ambient[2]=h;const I=n.hash;(I.directionalLength!==p||I.pointLength!==g||I.spotLength!==_||I.rectAreaLength!==m||I.hemiLength!==f||I.numDirectionalShadows!==E||I.numPointShadows!==b||I.numSpotShadows!==v||I.numSpotMaps!==T||I.numLightProbes!==C)&&(n.directional.length=p,n.spot.length=_,n.rectArea.length=m,n.point.length=g,n.hemi.length=f,n.directionalShadow.length=E,n.directionalShadowMap.length=E,n.pointShadow.length=b,n.pointShadowMap.length=b,n.spotShadow.length=v,n.spotShadowMap.length=v,n.directionalShadowMatrix.length=E,n.pointShadowMatrix.length=b,n.spotLightMatrix.length=v+T-A,n.spotLightMap.length=T,n.numSpotLightShadowsWithMaps=A,n.numLightProbes=C,I.directionalLength=p,I.pointLength=g,I.spotLength=_,I.rectAreaLength=m,I.hemiLength=f,I.numDirectionalShadows=E,I.numPointShadows=b,I.numSpotShadows=v,I.numSpotMaps=T,I.numLightProbes=C,n.version=fv++)}function l(c,u){let d=0,h=0,p=0,g=0,_=0;const m=u.matrixWorldInverse;for(let f=0,E=c.length;f<E;f++){const b=c[f];if(b.isDirectionalLight){const v=n.directional[d];v.direction.setFromMatrixPosition(b.matrixWorld),s.setFromMatrixPosition(b.target.matrixWorld),v.direction.sub(s),v.direction.transformDirection(m),d++}else if(b.isSpotLight){const v=n.spot[p];v.position.setFromMatrixPosition(b.matrixWorld),v.position.applyMatrix4(m),v.direction.setFromMatrixPosition(b.matrixWorld),s.setFromMatrixPosition(b.target.matrixWorld),v.direction.sub(s),v.direction.transformDirection(m),p++}else if(b.isRectAreaLight){const v=n.rectArea[g];v.position.setFromMatrixPosition(b.matrixWorld),v.position.applyMatrix4(m),a.identity(),r.copy(b.matrixWorld),r.premultiply(m),a.extractRotation(r),v.halfWidth.set(b.width*.5,0,0),v.halfHeight.set(0,b.height*.5,0),v.halfWidth.applyMatrix4(a),v.halfHeight.applyMatrix4(a),g++}else if(b.isPointLight){const v=n.point[h];v.position.setFromMatrixPosition(b.matrixWorld),v.position.applyMatrix4(m),h++}else if(b.isHemisphereLight){const v=n.hemi[_];v.direction.setFromMatrixPosition(b.matrixWorld),v.direction.transformDirection(m),_++}}}return{setup:o,setupView:l,state:n}}function nc(i){const e=new mv(i),t=[],n=[];function s(u){c.camera=u,t.length=0,n.length=0}function r(u){t.push(u)}function a(u){n.push(u)}function o(){e.setup(t)}function l(u){e.setupView(t,u)}const c={lightsArray:t,shadowsArray:n,camera:null,lights:e,transmissionRenderTarget:{}};return{init:s,state:c,setupLights:o,setupLightsView:l,pushLight:r,pushShadow:a}}function gv(i){let e=new WeakMap;function t(s,r=0){const a=e.get(s);let o;return a===void 0?(o=new nc(i),e.set(s,[o])):r>=a.length?(o=new nc(i),a.push(o)):o=a[r],o}function n(){e=new WeakMap}return{get:t,dispose:n}}const _v=`void main() {
	gl_Position = vec4( position, 1.0 );
}`,vv=`uniform sampler2D shadow_pass;
uniform vec2 resolution;
uniform float radius;
#include <packing>
void main() {
	const float samples = float( VSM_SAMPLES );
	float mean = 0.0;
	float squared_mean = 0.0;
	float uvStride = samples <= 1.0 ? 0.0 : 2.0 / ( samples - 1.0 );
	float uvStart = samples <= 1.0 ? 0.0 : - 1.0;
	for ( float i = 0.0; i < samples; i ++ ) {
		float uvOffset = uvStart + i * uvStride;
		#ifdef HORIZONTAL_PASS
			vec2 distribution = unpackRGBATo2Half( texture2D( shadow_pass, ( gl_FragCoord.xy + vec2( uvOffset, 0.0 ) * radius ) / resolution ) );
			mean += distribution.x;
			squared_mean += distribution.y * distribution.y + distribution.x * distribution.x;
		#else
			float depth = unpackRGBAToDepth( texture2D( shadow_pass, ( gl_FragCoord.xy + vec2( 0.0, uvOffset ) * radius ) / resolution ) );
			mean += depth;
			squared_mean += depth * depth;
		#endif
	}
	mean = mean / samples;
	squared_mean = squared_mean / samples;
	float std_dev = sqrt( squared_mean - mean * mean );
	gl_FragColor = pack2HalfToRGBA( vec2( mean, std_dev ) );
}`;function xv(i,e,t){let n=new Ro;const s=new et,r=new et,a=new At,o=new Bf({depthPacking:Uh}),l=new zf,c={},u=t.maxTextureSize,d={[qn]:Yt,[Yt]:qn,[qt]:qt},h=new on({defines:{VSM_SAMPLES:8},uniforms:{shadow_pass:{value:null},resolution:{value:new et},radius:{value:4}},vertexShader:_v,fragmentShader:vv}),p=h.clone();p.defines.HORIZONTAL_PASS=1;const g=new jt;g.setAttribute("position",new en(new Float32Array([-1,-1,.5,3,-1,.5,-1,3,.5]),3));const _=new kt(g,h),m=this;this.enabled=!1,this.autoUpdate=!0,this.needsUpdate=!1,this.type=xc;let f=this.type;this.render=function(A,C,I){if(m.enabled===!1||m.autoUpdate===!1&&m.needsUpdate===!1||A.length===0)return;const y=i.getRenderTarget(),S=i.getActiveCubeFace(),P=i.getActiveMipmapLevel(),$=i.state;$.setBlending(Wn),$.buffers.depth.getReversed()?$.buffers.color.setClear(0,0,0,0):$.buffers.color.setClear(1,1,1,1),$.buffers.depth.setTest(!0),$.setScissorTest(!1);const q=f!==Rn&&this.type===Rn,Y=f===Rn&&this.type!==Rn;for(let X=0,B=A.length;X<B;X++){const Q=A[X],G=Q.shadow;if(G===void 0){console.warn("THREE.WebGLShadowMap:",Q,"has no shadow.");continue}if(G.autoUpdate===!1&&G.needsUpdate===!1)continue;s.copy(G.mapSize);const ie=G.getFrameExtents();if(s.multiply(ie),r.copy(G.mapSize),(s.x>u||s.y>u)&&(s.x>u&&(r.x=Math.floor(u/ie.x),s.x=r.x*ie.x,G.mapSize.x=r.x),s.y>u&&(r.y=Math.floor(u/ie.y),s.y=r.y*ie.y,G.mapSize.y=r.y)),G.map===null||q===!0||Y===!0){const Ee=this.type!==Rn?{minFilter:fn,magFilter:fn}:{};G.map!==null&&G.map.dispose(),G.map=new di(s.x,s.y,Ee),G.map.texture.name=Q.name+".shadowMap",G.camera.updateProjectionMatrix()}i.setRenderTarget(G.map),i.clear();const de=G.getViewportCount();for(let Ee=0;Ee<de;Ee++){const Oe=G.getViewport(Ee);a.set(r.x*Oe.x,r.y*Oe.y,r.x*Oe.z,r.y*Oe.w),$.viewport(a),G.updateMatrices(Q,Ee),n=G.getFrustum(),v(C,I,G.camera,Q,this.type)}G.isPointLightShadow!==!0&&this.type===Rn&&E(G,I),G.needsUpdate=!1}f=this.type,m.needsUpdate=!1,i.setRenderTarget(y,S,P)};function E(A,C){const I=e.update(_);h.defines.VSM_SAMPLES!==A.blurSamples&&(h.defines.VSM_SAMPLES=A.blurSamples,p.defines.VSM_SAMPLES=A.blurSamples,h.needsUpdate=!0,p.needsUpdate=!0),A.mapPass===null&&(A.mapPass=new di(s.x,s.y)),h.uniforms.shadow_pass.value=A.map.texture,h.uniforms.resolution.value=A.mapSize,h.uniforms.radius.value=A.radius,i.setRenderTarget(A.mapPass),i.clear(),i.renderBufferDirect(C,null,I,h,_,null),p.uniforms.shadow_pass.value=A.mapPass.texture,p.uniforms.resolution.value=A.mapSize,p.uniforms.radius.value=A.radius,i.setRenderTarget(A.map),i.clear(),i.renderBufferDirect(C,null,I,p,_,null)}function b(A,C,I,y){let S=null;const P=I.isPointLight===!0?A.customDistanceMaterial:A.customDepthMaterial;if(P!==void 0)S=P;else if(S=I.isPointLight===!0?l:o,i.localClippingEnabled&&C.clipShadows===!0&&Array.isArray(C.clippingPlanes)&&C.clippingPlanes.length!==0||C.displacementMap&&C.displacementScale!==0||C.alphaMap&&C.alphaTest>0||C.map&&C.alphaTest>0||C.alphaToCoverage===!0){const $=S.uuid,q=C.uuid;let Y=c[$];Y===void 0&&(Y={},c[$]=Y);let X=Y[q];X===void 0&&(X=S.clone(),Y[q]=X,C.addEventListener("dispose",T)),S=X}if(S.visible=C.visible,S.wireframe=C.wireframe,y===Rn?S.side=C.shadowSide!==null?C.shadowSide:C.side:S.side=C.shadowSide!==null?C.shadowSide:d[C.side],S.alphaMap=C.alphaMap,S.alphaTest=C.alphaToCoverage===!0?.5:C.alphaTest,S.map=C.map,S.clipShadows=C.clipShadows,S.clippingPlanes=C.clippingPlanes,S.clipIntersection=C.clipIntersection,S.displacementMap=C.displacementMap,S.displacementScale=C.displacementScale,S.displacementBias=C.displacementBias,S.wireframeLinewidth=C.wireframeLinewidth,S.linewidth=C.linewidth,I.isPointLight===!0&&S.isMeshDistanceMaterial===!0){const $=i.properties.get(S);$.light=I}return S}function v(A,C,I,y,S){if(A.visible===!1)return;if(A.layers.test(C.layers)&&(A.isMesh||A.isLine||A.isPoints)&&(A.castShadow||A.receiveShadow&&S===Rn)&&(!A.frustumCulled||n.intersectsObject(A))){A.modelViewMatrix.multiplyMatrices(I.matrixWorldInverse,A.matrixWorld);const q=e.update(A),Y=A.material;if(Array.isArray(Y)){const X=q.groups;for(let B=0,Q=X.length;B<Q;B++){const G=X[B],ie=Y[G.materialIndex];if(ie&&ie.visible){const de=b(A,ie,y,S);A.onBeforeShadow(i,A,C,I,q,de,G),i.renderBufferDirect(I,null,q,de,A,G),A.onAfterShadow(i,A,C,I,q,de,G)}}}else if(Y.visible){const X=b(A,Y,y,S);A.onBeforeShadow(i,A,C,I,q,X,null),i.renderBufferDirect(I,null,q,X,A,null),A.onAfterShadow(i,A,C,I,q,X,null)}}const $=A.children;for(let q=0,Y=$.length;q<Y;q++)v($[q],C,I,y,S)}function T(A){A.target.removeEventListener("dispose",T);for(const I in c){const y=c[I],S=A.target.uuid;S in y&&(y[S].dispose(),delete y[S])}}}const Mv={[Ma]:Sa,[ya]:Ta,[ba]:wa,[Bi]:Ea,[Sa]:Ma,[Ta]:ya,[wa]:ba,[Ea]:Bi};function Sv(i,e){function t(){let U=!1;const oe=new At;let ue=null;const Me=new At(0,0,0,0);return{setMask:function(se){ue!==se&&!U&&(i.colorMask(se,se,se,se),ue=se)},setLocked:function(se){U=se},setClear:function(se,ee,Te,Ye,xt){xt===!0&&(se*=Ye,ee*=Ye,Te*=Ye),oe.set(se,ee,Te,Ye),Me.equals(oe)===!1&&(i.clearColor(se,ee,Te,Ye),Me.copy(oe))},reset:function(){U=!1,ue=null,Me.set(-1,0,0,0)}}}function n(){let U=!1,oe=!1,ue=null,Me=null,se=null;return{setReversed:function(ee){if(oe!==ee){const Te=e.get("EXT_clip_control");ee?Te.clipControlEXT(Te.LOWER_LEFT_EXT,Te.ZERO_TO_ONE_EXT):Te.clipControlEXT(Te.LOWER_LEFT_EXT,Te.NEGATIVE_ONE_TO_ONE_EXT),oe=ee;const Ye=se;se=null,this.setClear(Ye)}},getReversed:function(){return oe},setTest:function(ee){ee?le(i.DEPTH_TEST):Be(i.DEPTH_TEST)},setMask:function(ee){ue!==ee&&!U&&(i.depthMask(ee),ue=ee)},setFunc:function(ee){if(oe&&(ee=Mv[ee]),Me!==ee){switch(ee){case Ma:i.depthFunc(i.NEVER);break;case Sa:i.depthFunc(i.ALWAYS);break;case ya:i.depthFunc(i.LESS);break;case Bi:i.depthFunc(i.LEQUAL);break;case ba:i.depthFunc(i.EQUAL);break;case Ea:i.depthFunc(i.GEQUAL);break;case Ta:i.depthFunc(i.GREATER);break;case wa:i.depthFunc(i.NOTEQUAL);break;default:i.depthFunc(i.LEQUAL)}Me=ee}},setLocked:function(ee){U=ee},setClear:function(ee){se!==ee&&(oe&&(ee=1-ee),i.clearDepth(ee),se=ee)},reset:function(){U=!1,ue=null,Me=null,se=null,oe=!1}}}function s(){let U=!1,oe=null,ue=null,Me=null,se=null,ee=null,Te=null,Ye=null,xt=null;return{setTest:function(ut){U||(ut?le(i.STENCIL_TEST):Be(i.STENCIL_TEST))},setMask:function(ut){oe!==ut&&!U&&(i.stencilMask(ut),oe=ut)},setFunc:function(ut,yn,pn){(ue!==ut||Me!==yn||se!==pn)&&(i.stencilFunc(ut,yn,pn),ue=ut,Me=yn,se=pn)},setOp:function(ut,yn,pn){(ee!==ut||Te!==yn||Ye!==pn)&&(i.stencilOp(ut,yn,pn),ee=ut,Te=yn,Ye=pn)},setLocked:function(ut){U=ut},setClear:function(ut){xt!==ut&&(i.clearStencil(ut),xt=ut)},reset:function(){U=!1,oe=null,ue=null,Me=null,se=null,ee=null,Te=null,Ye=null,xt=null}}}const r=new t,a=new n,o=new s,l=new WeakMap,c=new WeakMap;let u={},d={},h=new WeakMap,p=[],g=null,_=!1,m=null,f=null,E=null,b=null,v=null,T=null,A=null,C=new Le(0,0,0),I=0,y=!1,S=null,P=null,$=null,q=null,Y=null;const X=i.getParameter(i.MAX_COMBINED_TEXTURE_IMAGE_UNITS);let B=!1,Q=0;const G=i.getParameter(i.VERSION);G.indexOf("WebGL")!==-1?(Q=parseFloat(/^WebGL (\d)/.exec(G)[1]),B=Q>=1):G.indexOf("OpenGL ES")!==-1&&(Q=parseFloat(/^OpenGL ES (\d)/.exec(G)[1]),B=Q>=2);let ie=null,de={};const Ee=i.getParameter(i.SCISSOR_BOX),Oe=i.getParameter(i.VIEWPORT),at=new At().fromArray(Ee),He=new At().fromArray(Oe);function j(U,oe,ue,Me){const se=new Uint8Array(4),ee=i.createTexture();i.bindTexture(U,ee),i.texParameteri(U,i.TEXTURE_MIN_FILTER,i.NEAREST),i.texParameteri(U,i.TEXTURE_MAG_FILTER,i.NEAREST);for(let Te=0;Te<ue;Te++)U===i.TEXTURE_3D||U===i.TEXTURE_2D_ARRAY?i.texImage3D(oe,0,i.RGBA,1,1,Me,0,i.RGBA,i.UNSIGNED_BYTE,se):i.texImage2D(oe+Te,0,i.RGBA,1,1,0,i.RGBA,i.UNSIGNED_BYTE,se);return ee}const he={};he[i.TEXTURE_2D]=j(i.TEXTURE_2D,i.TEXTURE_2D,1),he[i.TEXTURE_CUBE_MAP]=j(i.TEXTURE_CUBE_MAP,i.TEXTURE_CUBE_MAP_POSITIVE_X,6),he[i.TEXTURE_2D_ARRAY]=j(i.TEXTURE_2D_ARRAY,i.TEXTURE_2D_ARRAY,1,1),he[i.TEXTURE_3D]=j(i.TEXTURE_3D,i.TEXTURE_3D,1,1),r.setClear(0,0,0,1),a.setClear(1),o.setClear(0),le(i.DEPTH_TEST),a.setFunc(Bi),rt(!1),ke(el),le(i.CULL_FACE),ft(Wn);function le(U){u[U]!==!0&&(i.enable(U),u[U]=!0)}function Be(U){u[U]!==!1&&(i.disable(U),u[U]=!1)}function Ue(U,oe){return d[U]!==oe?(i.bindFramebuffer(U,oe),d[U]=oe,U===i.DRAW_FRAMEBUFFER&&(d[i.FRAMEBUFFER]=oe),U===i.FRAMEBUFFER&&(d[i.DRAW_FRAMEBUFFER]=oe),!0):!1}function $e(U,oe){let ue=p,Me=!1;if(U){ue=h.get(oe),ue===void 0&&(ue=[],h.set(oe,ue));const se=U.textures;if(ue.length!==se.length||ue[0]!==i.COLOR_ATTACHMENT0){for(let ee=0,Te=se.length;ee<Te;ee++)ue[ee]=i.COLOR_ATTACHMENT0+ee;ue.length=se.length,Me=!0}}else ue[0]!==i.BACK&&(ue[0]=i.BACK,Me=!0);Me&&i.drawBuffers(ue)}function Mt(U){return g!==U?(i.useProgram(U),g=U,!0):!1}const st={[ai]:i.FUNC_ADD,[ah]:i.FUNC_SUBTRACT,[oh]:i.FUNC_REVERSE_SUBTRACT};st[lh]=i.MIN,st[ch]=i.MAX;const D={[uh]:i.ZERO,[dh]:i.ONE,[hh]:i.SRC_COLOR,[va]:i.SRC_ALPHA,[vh]:i.SRC_ALPHA_SATURATE,[gh]:i.DST_COLOR,[ph]:i.DST_ALPHA,[fh]:i.ONE_MINUS_SRC_COLOR,[xa]:i.ONE_MINUS_SRC_ALPHA,[_h]:i.ONE_MINUS_DST_COLOR,[mh]:i.ONE_MINUS_DST_ALPHA,[xh]:i.CONSTANT_COLOR,[Mh]:i.ONE_MINUS_CONSTANT_COLOR,[Sh]:i.CONSTANT_ALPHA,[yh]:i.ONE_MINUS_CONSTANT_ALPHA};function ft(U,oe,ue,Me,se,ee,Te,Ye,xt,ut){if(U===Wn){_===!0&&(Be(i.BLEND),_=!1);return}if(_===!1&&(le(i.BLEND),_=!0),U!==rh){if(U!==m||ut!==y){if((f!==ai||v!==ai)&&(i.blendEquation(i.FUNC_ADD),f=ai,v=ai),ut)switch(U){case ki:i.blendFuncSeparate(i.ONE,i.ONE_MINUS_SRC_ALPHA,i.ONE,i.ONE_MINUS_SRC_ALPHA);break;case sr:i.blendFunc(i.ONE,i.ONE);break;case tl:i.blendFuncSeparate(i.ZERO,i.ONE_MINUS_SRC_COLOR,i.ZERO,i.ONE);break;case nl:i.blendFuncSeparate(i.DST_COLOR,i.ONE_MINUS_SRC_ALPHA,i.ZERO,i.ONE);break;default:console.error("THREE.WebGLState: Invalid blending: ",U);break}else switch(U){case ki:i.blendFuncSeparate(i.SRC_ALPHA,i.ONE_MINUS_SRC_ALPHA,i.ONE,i.ONE_MINUS_SRC_ALPHA);break;case sr:i.blendFuncSeparate(i.SRC_ALPHA,i.ONE,i.ONE,i.ONE);break;case tl:console.error("THREE.WebGLState: SubtractiveBlending requires material.premultipliedAlpha = true");break;case nl:console.error("THREE.WebGLState: MultiplyBlending requires material.premultipliedAlpha = true");break;default:console.error("THREE.WebGLState: Invalid blending: ",U);break}E=null,b=null,T=null,A=null,C.set(0,0,0),I=0,m=U,y=ut}return}se=se||oe,ee=ee||ue,Te=Te||Me,(oe!==f||se!==v)&&(i.blendEquationSeparate(st[oe],st[se]),f=oe,v=se),(ue!==E||Me!==b||ee!==T||Te!==A)&&(i.blendFuncSeparate(D[ue],D[Me],D[ee],D[Te]),E=ue,b=Me,T=ee,A=Te),(Ye.equals(C)===!1||xt!==I)&&(i.blendColor(Ye.r,Ye.g,Ye.b,xt),C.copy(Ye),I=xt),m=U,y=!1}function Ne(U,oe){U.side===qt?Be(i.CULL_FACE):le(i.CULL_FACE);let ue=U.side===Yt;oe&&(ue=!ue),rt(ue),U.blending===ki&&U.transparent===!1?ft(Wn):ft(U.blending,U.blendEquation,U.blendSrc,U.blendDst,U.blendEquationAlpha,U.blendSrcAlpha,U.blendDstAlpha,U.blendColor,U.blendAlpha,U.premultipliedAlpha),a.setFunc(U.depthFunc),a.setTest(U.depthTest),a.setMask(U.depthWrite),r.setMask(U.colorWrite);const Me=U.stencilWrite;o.setTest(Me),Me&&(o.setMask(U.stencilWriteMask),o.setFunc(U.stencilFunc,U.stencilRef,U.stencilFuncMask),o.setOp(U.stencilFail,U.stencilZFail,U.stencilZPass)),Se(U.polygonOffset,U.polygonOffsetFactor,U.polygonOffsetUnits),U.alphaToCoverage===!0?le(i.SAMPLE_ALPHA_TO_COVERAGE):Be(i.SAMPLE_ALPHA_TO_COVERAGE)}function rt(U){S!==U&&(U?i.frontFace(i.CW):i.frontFace(i.CCW),S=U)}function ke(U){U!==ih?(le(i.CULL_FACE),U!==P&&(U===el?i.cullFace(i.BACK):U===sh?i.cullFace(i.FRONT):i.cullFace(i.FRONT_AND_BACK))):Be(i.CULL_FACE),P=U}function yt(U){U!==$&&(B&&i.lineWidth(U),$=U)}function Se(U,oe,ue){U?(le(i.POLYGON_OFFSET_FILL),(q!==oe||Y!==ue)&&(i.polygonOffset(oe,ue),q=oe,Y=ue)):Be(i.POLYGON_OFFSET_FILL)}function Je(U){U?le(i.SCISSOR_TEST):Be(i.SCISSOR_TEST)}function Dt(U){U===void 0&&(U=i.TEXTURE0+X-1),ie!==U&&(i.activeTexture(U),ie=U)}function Rt(U,oe,ue){ue===void 0&&(ie===null?ue=i.TEXTURE0+X-1:ue=ie);let Me=de[ue];Me===void 0&&(Me={type:void 0,texture:void 0},de[ue]=Me),(Me.type!==U||Me.texture!==oe)&&(ie!==ue&&(i.activeTexture(ue),ie=ue),i.bindTexture(U,oe||he[U]),Me.type=U,Me.texture=oe)}function R(){const U=de[ie];U!==void 0&&U.type!==void 0&&(i.bindTexture(U.type,null),U.type=void 0,U.texture=void 0)}function x(){try{i.compressedTexImage2D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function z(){try{i.compressedTexImage3D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function K(){try{i.texSubImage2D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function ne(){try{i.texSubImage3D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function Z(){try{i.compressedTexSubImage2D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function Pe(){try{i.compressedTexSubImage3D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function ce(){try{i.texStorage2D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function we(){try{i.texStorage3D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function Ae(){try{i.texImage2D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function ae(){try{i.texImage3D(...arguments)}catch(U){console.error("THREE.WebGLState:",U)}}function ge(U){at.equals(U)===!1&&(i.scissor(U.x,U.y,U.z,U.w),at.copy(U))}function Ge(U){He.equals(U)===!1&&(i.viewport(U.x,U.y,U.z,U.w),He.copy(U))}function Re(U,oe){let ue=c.get(oe);ue===void 0&&(ue=new WeakMap,c.set(oe,ue));let Me=ue.get(U);Me===void 0&&(Me=i.getUniformBlockIndex(oe,U.name),ue.set(U,Me))}function pe(U,oe){const Me=c.get(oe).get(U);l.get(oe)!==Me&&(i.uniformBlockBinding(oe,Me,U.__bindingPointIndex),l.set(oe,Me))}function je(){i.disable(i.BLEND),i.disable(i.CULL_FACE),i.disable(i.DEPTH_TEST),i.disable(i.POLYGON_OFFSET_FILL),i.disable(i.SCISSOR_TEST),i.disable(i.STENCIL_TEST),i.disable(i.SAMPLE_ALPHA_TO_COVERAGE),i.blendEquation(i.FUNC_ADD),i.blendFunc(i.ONE,i.ZERO),i.blendFuncSeparate(i.ONE,i.ZERO,i.ONE,i.ZERO),i.blendColor(0,0,0,0),i.colorMask(!0,!0,!0,!0),i.clearColor(0,0,0,0),i.depthMask(!0),i.depthFunc(i.LESS),a.setReversed(!1),i.clearDepth(1),i.stencilMask(4294967295),i.stencilFunc(i.ALWAYS,0,4294967295),i.stencilOp(i.KEEP,i.KEEP,i.KEEP),i.clearStencil(0),i.cullFace(i.BACK),i.frontFace(i.CCW),i.polygonOffset(0,0),i.activeTexture(i.TEXTURE0),i.bindFramebuffer(i.FRAMEBUFFER,null),i.bindFramebuffer(i.DRAW_FRAMEBUFFER,null),i.bindFramebuffer(i.READ_FRAMEBUFFER,null),i.useProgram(null),i.lineWidth(1),i.scissor(0,0,i.canvas.width,i.canvas.height),i.viewport(0,0,i.canvas.width,i.canvas.height),u={},ie=null,de={},d={},h=new WeakMap,p=[],g=null,_=!1,m=null,f=null,E=null,b=null,v=null,T=null,A=null,C=new Le(0,0,0),I=0,y=!1,S=null,P=null,$=null,q=null,Y=null,at.set(0,0,i.canvas.width,i.canvas.height),He.set(0,0,i.canvas.width,i.canvas.height),r.reset(),a.reset(),o.reset()}return{buffers:{color:r,depth:a,stencil:o},enable:le,disable:Be,bindFramebuffer:Ue,drawBuffers:$e,useProgram:Mt,setBlending:ft,setMaterial:Ne,setFlipSided:rt,setCullFace:ke,setLineWidth:yt,setPolygonOffset:Se,setScissorTest:Je,activeTexture:Dt,bindTexture:Rt,unbindTexture:R,compressedTexImage2D:x,compressedTexImage3D:z,texImage2D:Ae,texImage3D:ae,updateUBOMapping:Re,uniformBlockBinding:pe,texStorage2D:ce,texStorage3D:we,texSubImage2D:K,texSubImage3D:ne,compressedTexSubImage2D:Z,compressedTexSubImage3D:Pe,scissor:ge,viewport:Ge,reset:je}}function yv(i,e,t,n,s,r,a){const o=e.has("WEBGL_multisampled_render_to_texture")?e.get("WEBGL_multisampled_render_to_texture"):null,l=typeof navigator>"u"?!1:/OculusBrowser/g.test(navigator.userAgent),c=new et,u=new WeakMap;let d;const h=new WeakMap;let p=!1;try{p=typeof OffscreenCanvas<"u"&&new OffscreenCanvas(1,1).getContext("2d")!==null}catch{}function g(R,x){return p?new OffscreenCanvas(R,x):ds("canvas")}function _(R,x,z){let K=1;const ne=Rt(R);if((ne.width>z||ne.height>z)&&(K=z/Math.max(ne.width,ne.height)),K<1)if(typeof HTMLImageElement<"u"&&R instanceof HTMLImageElement||typeof HTMLCanvasElement<"u"&&R instanceof HTMLCanvasElement||typeof ImageBitmap<"u"&&R instanceof ImageBitmap||typeof VideoFrame<"u"&&R instanceof VideoFrame){const Z=Math.floor(K*ne.width),Pe=Math.floor(K*ne.height);d===void 0&&(d=g(Z,Pe));const ce=x?g(Z,Pe):d;return ce.width=Z,ce.height=Pe,ce.getContext("2d").drawImage(R,0,0,Z,Pe),console.warn("THREE.WebGLRenderer: Texture has been resized from ("+ne.width+"x"+ne.height+") to ("+Z+"x"+Pe+")."),ce}else return"data"in R&&console.warn("THREE.WebGLRenderer: Image in DataTexture is too big ("+ne.width+"x"+ne.height+")."),R;return R}function m(R){return R.generateMipmaps}function f(R){i.generateMipmap(R)}function E(R){return R.isWebGLCubeRenderTarget?i.TEXTURE_CUBE_MAP:R.isWebGL3DRenderTarget?i.TEXTURE_3D:R.isWebGLArrayRenderTarget||R.isCompressedArrayTexture?i.TEXTURE_2D_ARRAY:i.TEXTURE_2D}function b(R,x,z,K,ne=!1){if(R!==null){if(i[R]!==void 0)return i[R];console.warn("THREE.WebGLRenderer: Attempt to use non-existing WebGL internal format '"+R+"'")}let Z=x;if(x===i.RED&&(z===i.FLOAT&&(Z=i.R32F),z===i.HALF_FLOAT&&(Z=i.R16F),z===i.UNSIGNED_BYTE&&(Z=i.R8)),x===i.RED_INTEGER&&(z===i.UNSIGNED_BYTE&&(Z=i.R8UI),z===i.UNSIGNED_SHORT&&(Z=i.R16UI),z===i.UNSIGNED_INT&&(Z=i.R32UI),z===i.BYTE&&(Z=i.R8I),z===i.SHORT&&(Z=i.R16I),z===i.INT&&(Z=i.R32I)),x===i.RG&&(z===i.FLOAT&&(Z=i.RG32F),z===i.HALF_FLOAT&&(Z=i.RG16F),z===i.UNSIGNED_BYTE&&(Z=i.RG8)),x===i.RG_INTEGER&&(z===i.UNSIGNED_BYTE&&(Z=i.RG8UI),z===i.UNSIGNED_SHORT&&(Z=i.RG16UI),z===i.UNSIGNED_INT&&(Z=i.RG32UI),z===i.BYTE&&(Z=i.RG8I),z===i.SHORT&&(Z=i.RG16I),z===i.INT&&(Z=i.RG32I)),x===i.RGB_INTEGER&&(z===i.UNSIGNED_BYTE&&(Z=i.RGB8UI),z===i.UNSIGNED_SHORT&&(Z=i.RGB16UI),z===i.UNSIGNED_INT&&(Z=i.RGB32UI),z===i.BYTE&&(Z=i.RGB8I),z===i.SHORT&&(Z=i.RGB16I),z===i.INT&&(Z=i.RGB32I)),x===i.RGBA_INTEGER&&(z===i.UNSIGNED_BYTE&&(Z=i.RGBA8UI),z===i.UNSIGNED_SHORT&&(Z=i.RGBA16UI),z===i.UNSIGNED_INT&&(Z=i.RGBA32UI),z===i.BYTE&&(Z=i.RGBA8I),z===i.SHORT&&(Z=i.RGBA16I),z===i.INT&&(Z=i.RGBA32I)),x===i.RGB&&z===i.UNSIGNED_INT_5_9_9_9_REV&&(Z=i.RGB9_E5),x===i.RGBA){const Pe=ne?rr:lt.getTransfer(K);z===i.FLOAT&&(Z=i.RGBA32F),z===i.HALF_FLOAT&&(Z=i.RGBA16F),z===i.UNSIGNED_BYTE&&(Z=Pe===mt?i.SRGB8_ALPHA8:i.RGBA8),z===i.UNSIGNED_SHORT_4_4_4_4&&(Z=i.RGBA4),z===i.UNSIGNED_SHORT_5_5_5_1&&(Z=i.RGB5_A1)}return(Z===i.R16F||Z===i.R32F||Z===i.RG16F||Z===i.RG32F||Z===i.RGBA16F||Z===i.RGBA32F)&&e.get("EXT_color_buffer_float"),Z}function v(R,x){let z;return R?x===null||x===ui||x===os?z=i.DEPTH24_STENCIL8:x===Pn?z=i.DEPTH32F_STENCIL8:x===as&&(z=i.DEPTH24_STENCIL8,console.warn("DepthTexture: 16 bit depth attachment is not supported with stencil. Using 24-bit attachment.")):x===null||x===ui||x===os?z=i.DEPTH_COMPONENT24:x===Pn?z=i.DEPTH_COMPONENT32F:x===as&&(z=i.DEPTH_COMPONENT16),z}function T(R,x){return m(R)===!0||R.isFramebufferTexture&&R.minFilter!==fn&&R.minFilter!==an?Math.log2(Math.max(x.width,x.height))+1:R.mipmaps!==void 0&&R.mipmaps.length>0?R.mipmaps.length:R.isCompressedTexture&&Array.isArray(R.image)?x.mipmaps.length:1}function A(R){const x=R.target;x.removeEventListener("dispose",A),I(x),x.isVideoTexture&&u.delete(x)}function C(R){const x=R.target;x.removeEventListener("dispose",C),S(x)}function I(R){const x=n.get(R);if(x.__webglInit===void 0)return;const z=R.source,K=h.get(z);if(K){const ne=K[x.__cacheKey];ne.usedTimes--,ne.usedTimes===0&&y(R),Object.keys(K).length===0&&h.delete(z)}n.remove(R)}function y(R){const x=n.get(R);i.deleteTexture(x.__webglTexture);const z=R.source,K=h.get(z);delete K[x.__cacheKey],a.memory.textures--}function S(R){const x=n.get(R);if(R.depthTexture&&(R.depthTexture.dispose(),n.remove(R.depthTexture)),R.isWebGLCubeRenderTarget)for(let K=0;K<6;K++){if(Array.isArray(x.__webglFramebuffer[K]))for(let ne=0;ne<x.__webglFramebuffer[K].length;ne++)i.deleteFramebuffer(x.__webglFramebuffer[K][ne]);else i.deleteFramebuffer(x.__webglFramebuffer[K]);x.__webglDepthbuffer&&i.deleteRenderbuffer(x.__webglDepthbuffer[K])}else{if(Array.isArray(x.__webglFramebuffer))for(let K=0;K<x.__webglFramebuffer.length;K++)i.deleteFramebuffer(x.__webglFramebuffer[K]);else i.deleteFramebuffer(x.__webglFramebuffer);if(x.__webglDepthbuffer&&i.deleteRenderbuffer(x.__webglDepthbuffer),x.__webglMultisampledFramebuffer&&i.deleteFramebuffer(x.__webglMultisampledFramebuffer),x.__webglColorRenderbuffer)for(let K=0;K<x.__webglColorRenderbuffer.length;K++)x.__webglColorRenderbuffer[K]&&i.deleteRenderbuffer(x.__webglColorRenderbuffer[K]);x.__webglDepthRenderbuffer&&i.deleteRenderbuffer(x.__webglDepthRenderbuffer)}const z=R.textures;for(let K=0,ne=z.length;K<ne;K++){const Z=n.get(z[K]);Z.__webglTexture&&(i.deleteTexture(Z.__webglTexture),a.memory.textures--),n.remove(z[K])}n.remove(R)}let P=0;function $(){P=0}function q(){const R=P;return R>=s.maxTextures&&console.warn("THREE.WebGLTextures: Trying to use "+R+" texture units while this GPU supports only "+s.maxTextures),P+=1,R}function Y(R){const x=[];return x.push(R.wrapS),x.push(R.wrapT),x.push(R.wrapR||0),x.push(R.magFilter),x.push(R.minFilter),x.push(R.anisotropy),x.push(R.internalFormat),x.push(R.format),x.push(R.type),x.push(R.generateMipmaps),x.push(R.premultiplyAlpha),x.push(R.flipY),x.push(R.unpackAlignment),x.push(R.colorSpace),x.join()}function X(R,x){const z=n.get(R);if(R.isVideoTexture&&Je(R),R.isRenderTargetTexture===!1&&R.isExternalTexture!==!0&&R.version>0&&z.__version!==R.version){const K=R.image;if(K===null)console.warn("THREE.WebGLRenderer: Texture marked for update but no image data found.");else if(K.complete===!1)console.warn("THREE.WebGLRenderer: Texture marked for update but image is incomplete");else{he(z,R,x);return}}else R.isExternalTexture&&(z.__webglTexture=R.sourceTexture?R.sourceTexture:null);t.bindTexture(i.TEXTURE_2D,z.__webglTexture,i.TEXTURE0+x)}function B(R,x){const z=n.get(R);if(R.isRenderTargetTexture===!1&&R.version>0&&z.__version!==R.version){he(z,R,x);return}t.bindTexture(i.TEXTURE_2D_ARRAY,z.__webglTexture,i.TEXTURE0+x)}function Q(R,x){const z=n.get(R);if(R.isRenderTargetTexture===!1&&R.version>0&&z.__version!==R.version){he(z,R,x);return}t.bindTexture(i.TEXTURE_3D,z.__webglTexture,i.TEXTURE0+x)}function G(R,x){const z=n.get(R);if(R.version>0&&z.__version!==R.version){le(z,R,x);return}t.bindTexture(i.TEXTURE_CUBE_MAP,z.__webglTexture,i.TEXTURE0+x)}const ie={[Ca]:i.REPEAT,[Cn]:i.CLAMP_TO_EDGE,[Pa]:i.MIRRORED_REPEAT},de={[fn]:i.NEAREST,[Dh]:i.NEAREST_MIPMAP_NEAREST,[ys]:i.NEAREST_MIPMAP_LINEAR,[an]:i.LINEAR,[Rr]:i.LINEAR_MIPMAP_NEAREST,[li]:i.LINEAR_MIPMAP_LINEAR},Ee={[Nh]:i.NEVER,[Hh]:i.ALWAYS,[Fh]:i.LESS,[Pc]:i.LEQUAL,[Oh]:i.EQUAL,[Vh]:i.GEQUAL,[Bh]:i.GREATER,[zh]:i.NOTEQUAL};function Oe(R,x){if(x.type===Pn&&e.has("OES_texture_float_linear")===!1&&(x.magFilter===an||x.magFilter===Rr||x.magFilter===ys||x.magFilter===li||x.minFilter===an||x.minFilter===Rr||x.minFilter===ys||x.minFilter===li)&&console.warn("THREE.WebGLRenderer: Unable to use linear filtering with floating point textures. OES_texture_float_linear not supported on this device."),i.texParameteri(R,i.TEXTURE_WRAP_S,ie[x.wrapS]),i.texParameteri(R,i.TEXTURE_WRAP_T,ie[x.wrapT]),(R===i.TEXTURE_3D||R===i.TEXTURE_2D_ARRAY)&&i.texParameteri(R,i.TEXTURE_WRAP_R,ie[x.wrapR]),i.texParameteri(R,i.TEXTURE_MAG_FILTER,de[x.magFilter]),i.texParameteri(R,i.TEXTURE_MIN_FILTER,de[x.minFilter]),x.compareFunction&&(i.texParameteri(R,i.TEXTURE_COMPARE_MODE,i.COMPARE_REF_TO_TEXTURE),i.texParameteri(R,i.TEXTURE_COMPARE_FUNC,Ee[x.compareFunction])),e.has("EXT_texture_filter_anisotropic")===!0){if(x.magFilter===fn||x.minFilter!==ys&&x.minFilter!==li||x.type===Pn&&e.has("OES_texture_float_linear")===!1)return;if(x.anisotropy>1||n.get(x).__currentAnisotropy){const z=e.get("EXT_texture_filter_anisotropic");i.texParameterf(R,z.TEXTURE_MAX_ANISOTROPY_EXT,Math.min(x.anisotropy,s.getMaxAnisotropy())),n.get(x).__currentAnisotropy=x.anisotropy}}}function at(R,x){let z=!1;R.__webglInit===void 0&&(R.__webglInit=!0,x.addEventListener("dispose",A));const K=x.source;let ne=h.get(K);ne===void 0&&(ne={},h.set(K,ne));const Z=Y(x);if(Z!==R.__cacheKey){ne[Z]===void 0&&(ne[Z]={texture:i.createTexture(),usedTimes:0},a.memory.textures++,z=!0),ne[Z].usedTimes++;const Pe=ne[R.__cacheKey];Pe!==void 0&&(ne[R.__cacheKey].usedTimes--,Pe.usedTimes===0&&y(x)),R.__cacheKey=Z,R.__webglTexture=ne[Z].texture}return z}function He(R,x,z){return Math.floor(Math.floor(R/z)/x)}function j(R,x,z,K){const Z=R.updateRanges;if(Z.length===0)t.texSubImage2D(i.TEXTURE_2D,0,0,0,x.width,x.height,z,K,x.data);else{Z.sort((ae,ge)=>ae.start-ge.start);let Pe=0;for(let ae=1;ae<Z.length;ae++){const ge=Z[Pe],Ge=Z[ae],Re=ge.start+ge.count,pe=He(Ge.start,x.width,4),je=He(ge.start,x.width,4);Ge.start<=Re+1&&pe===je&&He(Ge.start+Ge.count-1,x.width,4)===pe?ge.count=Math.max(ge.count,Ge.start+Ge.count-ge.start):(++Pe,Z[Pe]=Ge)}Z.length=Pe+1;const ce=i.getParameter(i.UNPACK_ROW_LENGTH),we=i.getParameter(i.UNPACK_SKIP_PIXELS),Ae=i.getParameter(i.UNPACK_SKIP_ROWS);i.pixelStorei(i.UNPACK_ROW_LENGTH,x.width);for(let ae=0,ge=Z.length;ae<ge;ae++){const Ge=Z[ae],Re=Math.floor(Ge.start/4),pe=Math.ceil(Ge.count/4),je=Re%x.width,U=Math.floor(Re/x.width),oe=pe,ue=1;i.pixelStorei(i.UNPACK_SKIP_PIXELS,je),i.pixelStorei(i.UNPACK_SKIP_ROWS,U),t.texSubImage2D(i.TEXTURE_2D,0,je,U,oe,ue,z,K,x.data)}R.clearUpdateRanges(),i.pixelStorei(i.UNPACK_ROW_LENGTH,ce),i.pixelStorei(i.UNPACK_SKIP_PIXELS,we),i.pixelStorei(i.UNPACK_SKIP_ROWS,Ae)}}function he(R,x,z){let K=i.TEXTURE_2D;(x.isDataArrayTexture||x.isCompressedArrayTexture)&&(K=i.TEXTURE_2D_ARRAY),x.isData3DTexture&&(K=i.TEXTURE_3D);const ne=at(R,x),Z=x.source;t.bindTexture(K,R.__webglTexture,i.TEXTURE0+z);const Pe=n.get(Z);if(Z.version!==Pe.__version||ne===!0){t.activeTexture(i.TEXTURE0+z);const ce=lt.getPrimaries(lt.workingColorSpace),we=x.colorSpace===Hn?null:lt.getPrimaries(x.colorSpace),Ae=x.colorSpace===Hn||ce===we?i.NONE:i.BROWSER_DEFAULT_WEBGL;i.pixelStorei(i.UNPACK_FLIP_Y_WEBGL,x.flipY),i.pixelStorei(i.UNPACK_PREMULTIPLY_ALPHA_WEBGL,x.premultiplyAlpha),i.pixelStorei(i.UNPACK_ALIGNMENT,x.unpackAlignment),i.pixelStorei(i.UNPACK_COLORSPACE_CONVERSION_WEBGL,Ae);let ae=_(x.image,!1,s.maxTextureSize);ae=Dt(x,ae);const ge=r.convert(x.format,x.colorSpace),Ge=r.convert(x.type);let Re=b(x.internalFormat,ge,Ge,x.colorSpace,x.isVideoTexture);Oe(K,x);let pe;const je=x.mipmaps,U=x.isVideoTexture!==!0,oe=Pe.__version===void 0||ne===!0,ue=Z.dataReady,Me=T(x,ae);if(x.isDepthTexture)Re=v(x.format===cs,x.type),oe&&(U?t.texStorage2D(i.TEXTURE_2D,1,Re,ae.width,ae.height):t.texImage2D(i.TEXTURE_2D,0,Re,ae.width,ae.height,0,ge,Ge,null));else if(x.isDataTexture)if(je.length>0){U&&oe&&t.texStorage2D(i.TEXTURE_2D,Me,Re,je[0].width,je[0].height);for(let se=0,ee=je.length;se<ee;se++)pe=je[se],U?ue&&t.texSubImage2D(i.TEXTURE_2D,se,0,0,pe.width,pe.height,ge,Ge,pe.data):t.texImage2D(i.TEXTURE_2D,se,Re,pe.width,pe.height,0,ge,Ge,pe.data);x.generateMipmaps=!1}else U?(oe&&t.texStorage2D(i.TEXTURE_2D,Me,Re,ae.width,ae.height),ue&&j(x,ae,ge,Ge)):t.texImage2D(i.TEXTURE_2D,0,Re,ae.width,ae.height,0,ge,Ge,ae.data);else if(x.isCompressedTexture)if(x.isCompressedArrayTexture){U&&oe&&t.texStorage3D(i.TEXTURE_2D_ARRAY,Me,Re,je[0].width,je[0].height,ae.depth);for(let se=0,ee=je.length;se<ee;se++)if(pe=je[se],x.format!==hn)if(ge!==null)if(U){if(ue)if(x.layerUpdates.size>0){const Te=Dl(pe.width,pe.height,x.format,x.type);for(const Ye of x.layerUpdates){const xt=pe.data.subarray(Ye*Te/pe.data.BYTES_PER_ELEMENT,(Ye+1)*Te/pe.data.BYTES_PER_ELEMENT);t.compressedTexSubImage3D(i.TEXTURE_2D_ARRAY,se,0,0,Ye,pe.width,pe.height,1,ge,xt)}x.clearLayerUpdates()}else t.compressedTexSubImage3D(i.TEXTURE_2D_ARRAY,se,0,0,0,pe.width,pe.height,ae.depth,ge,pe.data)}else t.compressedTexImage3D(i.TEXTURE_2D_ARRAY,se,Re,pe.width,pe.height,ae.depth,0,pe.data,0,0);else console.warn("THREE.WebGLRenderer: Attempt to load unsupported compressed texture format in .uploadTexture()");else U?ue&&t.texSubImage3D(i.TEXTURE_2D_ARRAY,se,0,0,0,pe.width,pe.height,ae.depth,ge,Ge,pe.data):t.texImage3D(i.TEXTURE_2D_ARRAY,se,Re,pe.width,pe.height,ae.depth,0,ge,Ge,pe.data)}else{U&&oe&&t.texStorage2D(i.TEXTURE_2D,Me,Re,je[0].width,je[0].height);for(let se=0,ee=je.length;se<ee;se++)pe=je[se],x.format!==hn?ge!==null?U?ue&&t.compressedTexSubImage2D(i.TEXTURE_2D,se,0,0,pe.width,pe.height,ge,pe.data):t.compressedTexImage2D(i.TEXTURE_2D,se,Re,pe.width,pe.height,0,pe.data):console.warn("THREE.WebGLRenderer: Attempt to load unsupported compressed texture format in .uploadTexture()"):U?ue&&t.texSubImage2D(i.TEXTURE_2D,se,0,0,pe.width,pe.height,ge,Ge,pe.data):t.texImage2D(i.TEXTURE_2D,se,Re,pe.width,pe.height,0,ge,Ge,pe.data)}else if(x.isDataArrayTexture)if(U){if(oe&&t.texStorage3D(i.TEXTURE_2D_ARRAY,Me,Re,ae.width,ae.height,ae.depth),ue)if(x.layerUpdates.size>0){const se=Dl(ae.width,ae.height,x.format,x.type);for(const ee of x.layerUpdates){const Te=ae.data.subarray(ee*se/ae.data.BYTES_PER_ELEMENT,(ee+1)*se/ae.data.BYTES_PER_ELEMENT);t.texSubImage3D(i.TEXTURE_2D_ARRAY,0,0,0,ee,ae.width,ae.height,1,ge,Ge,Te)}x.clearLayerUpdates()}else t.texSubImage3D(i.TEXTURE_2D_ARRAY,0,0,0,0,ae.width,ae.height,ae.depth,ge,Ge,ae.data)}else t.texImage3D(i.TEXTURE_2D_ARRAY,0,Re,ae.width,ae.height,ae.depth,0,ge,Ge,ae.data);else if(x.isData3DTexture)U?(oe&&t.texStorage3D(i.TEXTURE_3D,Me,Re,ae.width,ae.height,ae.depth),ue&&t.texSubImage3D(i.TEXTURE_3D,0,0,0,0,ae.width,ae.height,ae.depth,ge,Ge,ae.data)):t.texImage3D(i.TEXTURE_3D,0,Re,ae.width,ae.height,ae.depth,0,ge,Ge,ae.data);else if(x.isFramebufferTexture){if(oe)if(U)t.texStorage2D(i.TEXTURE_2D,Me,Re,ae.width,ae.height);else{let se=ae.width,ee=ae.height;for(let Te=0;Te<Me;Te++)t.texImage2D(i.TEXTURE_2D,Te,Re,se,ee,0,ge,Ge,null),se>>=1,ee>>=1}}else if(je.length>0){if(U&&oe){const se=Rt(je[0]);t.texStorage2D(i.TEXTURE_2D,Me,Re,se.width,se.height)}for(let se=0,ee=je.length;se<ee;se++)pe=je[se],U?ue&&t.texSubImage2D(i.TEXTURE_2D,se,0,0,ge,Ge,pe):t.texImage2D(i.TEXTURE_2D,se,Re,ge,Ge,pe);x.generateMipmaps=!1}else if(U){if(oe){const se=Rt(ae);t.texStorage2D(i.TEXTURE_2D,Me,Re,se.width,se.height)}ue&&t.texSubImage2D(i.TEXTURE_2D,0,0,0,ge,Ge,ae)}else t.texImage2D(i.TEXTURE_2D,0,Re,ge,Ge,ae);m(x)&&f(K),Pe.__version=Z.version,x.onUpdate&&x.onUpdate(x)}R.__version=x.version}function le(R,x,z){if(x.image.length!==6)return;const K=at(R,x),ne=x.source;t.bindTexture(i.TEXTURE_CUBE_MAP,R.__webglTexture,i.TEXTURE0+z);const Z=n.get(ne);if(ne.version!==Z.__version||K===!0){t.activeTexture(i.TEXTURE0+z);const Pe=lt.getPrimaries(lt.workingColorSpace),ce=x.colorSpace===Hn?null:lt.getPrimaries(x.colorSpace),we=x.colorSpace===Hn||Pe===ce?i.NONE:i.BROWSER_DEFAULT_WEBGL;i.pixelStorei(i.UNPACK_FLIP_Y_WEBGL,x.flipY),i.pixelStorei(i.UNPACK_PREMULTIPLY_ALPHA_WEBGL,x.premultiplyAlpha),i.pixelStorei(i.UNPACK_ALIGNMENT,x.unpackAlignment),i.pixelStorei(i.UNPACK_COLORSPACE_CONVERSION_WEBGL,we);const Ae=x.isCompressedTexture||x.image[0].isCompressedTexture,ae=x.image[0]&&x.image[0].isDataTexture,ge=[];for(let ee=0;ee<6;ee++)!Ae&&!ae?ge[ee]=_(x.image[ee],!0,s.maxCubemapSize):ge[ee]=ae?x.image[ee].image:x.image[ee],ge[ee]=Dt(x,ge[ee]);const Ge=ge[0],Re=r.convert(x.format,x.colorSpace),pe=r.convert(x.type),je=b(x.internalFormat,Re,pe,x.colorSpace),U=x.isVideoTexture!==!0,oe=Z.__version===void 0||K===!0,ue=ne.dataReady;let Me=T(x,Ge);Oe(i.TEXTURE_CUBE_MAP,x);let se;if(Ae){U&&oe&&t.texStorage2D(i.TEXTURE_CUBE_MAP,Me,je,Ge.width,Ge.height);for(let ee=0;ee<6;ee++){se=ge[ee].mipmaps;for(let Te=0;Te<se.length;Te++){const Ye=se[Te];x.format!==hn?Re!==null?U?ue&&t.compressedTexSubImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te,0,0,Ye.width,Ye.height,Re,Ye.data):t.compressedTexImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te,je,Ye.width,Ye.height,0,Ye.data):console.warn("THREE.WebGLRenderer: Attempt to load unsupported compressed texture format in .setTextureCube()"):U?ue&&t.texSubImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te,0,0,Ye.width,Ye.height,Re,pe,Ye.data):t.texImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te,je,Ye.width,Ye.height,0,Re,pe,Ye.data)}}}else{if(se=x.mipmaps,U&&oe){se.length>0&&Me++;const ee=Rt(ge[0]);t.texStorage2D(i.TEXTURE_CUBE_MAP,Me,je,ee.width,ee.height)}for(let ee=0;ee<6;ee++)if(ae){U?ue&&t.texSubImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,0,0,0,ge[ee].width,ge[ee].height,Re,pe,ge[ee].data):t.texImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,0,je,ge[ee].width,ge[ee].height,0,Re,pe,ge[ee].data);for(let Te=0;Te<se.length;Te++){const xt=se[Te].image[ee].image;U?ue&&t.texSubImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te+1,0,0,xt.width,xt.height,Re,pe,xt.data):t.texImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te+1,je,xt.width,xt.height,0,Re,pe,xt.data)}}else{U?ue&&t.texSubImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,0,0,0,Re,pe,ge[ee]):t.texImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,0,je,Re,pe,ge[ee]);for(let Te=0;Te<se.length;Te++){const Ye=se[Te];U?ue&&t.texSubImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te+1,0,0,Re,pe,Ye.image[ee]):t.texImage2D(i.TEXTURE_CUBE_MAP_POSITIVE_X+ee,Te+1,je,Re,pe,Ye.image[ee])}}}m(x)&&f(i.TEXTURE_CUBE_MAP),Z.__version=ne.version,x.onUpdate&&x.onUpdate(x)}R.__version=x.version}function Be(R,x,z,K,ne,Z){const Pe=r.convert(z.format,z.colorSpace),ce=r.convert(z.type),we=b(z.internalFormat,Pe,ce,z.colorSpace),Ae=n.get(x),ae=n.get(z);if(ae.__renderTarget=x,!Ae.__hasExternalTextures){const ge=Math.max(1,x.width>>Z),Ge=Math.max(1,x.height>>Z);ne===i.TEXTURE_3D||ne===i.TEXTURE_2D_ARRAY?t.texImage3D(ne,Z,we,ge,Ge,x.depth,0,Pe,ce,null):t.texImage2D(ne,Z,we,ge,Ge,0,Pe,ce,null)}t.bindFramebuffer(i.FRAMEBUFFER,R),Se(x)?o.framebufferTexture2DMultisampleEXT(i.FRAMEBUFFER,K,ne,ae.__webglTexture,0,yt(x)):(ne===i.TEXTURE_2D||ne>=i.TEXTURE_CUBE_MAP_POSITIVE_X&&ne<=i.TEXTURE_CUBE_MAP_NEGATIVE_Z)&&i.framebufferTexture2D(i.FRAMEBUFFER,K,ne,ae.__webglTexture,Z),t.bindFramebuffer(i.FRAMEBUFFER,null)}function Ue(R,x,z){if(i.bindRenderbuffer(i.RENDERBUFFER,R),x.depthBuffer){const K=x.depthTexture,ne=K&&K.isDepthTexture?K.type:null,Z=v(x.stencilBuffer,ne),Pe=x.stencilBuffer?i.DEPTH_STENCIL_ATTACHMENT:i.DEPTH_ATTACHMENT,ce=yt(x);Se(x)?o.renderbufferStorageMultisampleEXT(i.RENDERBUFFER,ce,Z,x.width,x.height):z?i.renderbufferStorageMultisample(i.RENDERBUFFER,ce,Z,x.width,x.height):i.renderbufferStorage(i.RENDERBUFFER,Z,x.width,x.height),i.framebufferRenderbuffer(i.FRAMEBUFFER,Pe,i.RENDERBUFFER,R)}else{const K=x.textures;for(let ne=0;ne<K.length;ne++){const Z=K[ne],Pe=r.convert(Z.format,Z.colorSpace),ce=r.convert(Z.type),we=b(Z.internalFormat,Pe,ce,Z.colorSpace),Ae=yt(x);z&&Se(x)===!1?i.renderbufferStorageMultisample(i.RENDERBUFFER,Ae,we,x.width,x.height):Se(x)?o.renderbufferStorageMultisampleEXT(i.RENDERBUFFER,Ae,we,x.width,x.height):i.renderbufferStorage(i.RENDERBUFFER,we,x.width,x.height)}}i.bindRenderbuffer(i.RENDERBUFFER,null)}function $e(R,x){if(x&&x.isWebGLCubeRenderTarget)throw new Error("Depth Texture with cube render targets is not supported");if(t.bindFramebuffer(i.FRAMEBUFFER,R),!(x.depthTexture&&x.depthTexture.isDepthTexture))throw new Error("renderTarget.depthTexture must be an instance of THREE.DepthTexture");const K=n.get(x.depthTexture);K.__renderTarget=x,(!K.__webglTexture||x.depthTexture.image.width!==x.width||x.depthTexture.image.height!==x.height)&&(x.depthTexture.image.width=x.width,x.depthTexture.image.height=x.height,x.depthTexture.needsUpdate=!0),X(x.depthTexture,0);const ne=K.__webglTexture,Z=yt(x);if(x.depthTexture.format===ls)Se(x)?o.framebufferTexture2DMultisampleEXT(i.FRAMEBUFFER,i.DEPTH_ATTACHMENT,i.TEXTURE_2D,ne,0,Z):i.framebufferTexture2D(i.FRAMEBUFFER,i.DEPTH_ATTACHMENT,i.TEXTURE_2D,ne,0);else if(x.depthTexture.format===cs)Se(x)?o.framebufferTexture2DMultisampleEXT(i.FRAMEBUFFER,i.DEPTH_STENCIL_ATTACHMENT,i.TEXTURE_2D,ne,0,Z):i.framebufferTexture2D(i.FRAMEBUFFER,i.DEPTH_STENCIL_ATTACHMENT,i.TEXTURE_2D,ne,0);else throw new Error("Unknown depthTexture format")}function Mt(R){const x=n.get(R),z=R.isWebGLCubeRenderTarget===!0;if(x.__boundDepthTexture!==R.depthTexture){const K=R.depthTexture;if(x.__depthDisposeCallback&&x.__depthDisposeCallback(),K){const ne=()=>{delete x.__boundDepthTexture,delete x.__depthDisposeCallback,K.removeEventListener("dispose",ne)};K.addEventListener("dispose",ne),x.__depthDisposeCallback=ne}x.__boundDepthTexture=K}if(R.depthTexture&&!x.__autoAllocateDepthBuffer){if(z)throw new Error("target.depthTexture not supported in Cube render targets");const K=R.texture.mipmaps;K&&K.length>0?$e(x.__webglFramebuffer[0],R):$e(x.__webglFramebuffer,R)}else if(z){x.__webglDepthbuffer=[];for(let K=0;K<6;K++)if(t.bindFramebuffer(i.FRAMEBUFFER,x.__webglFramebuffer[K]),x.__webglDepthbuffer[K]===void 0)x.__webglDepthbuffer[K]=i.createRenderbuffer(),Ue(x.__webglDepthbuffer[K],R,!1);else{const ne=R.stencilBuffer?i.DEPTH_STENCIL_ATTACHMENT:i.DEPTH_ATTACHMENT,Z=x.__webglDepthbuffer[K];i.bindRenderbuffer(i.RENDERBUFFER,Z),i.framebufferRenderbuffer(i.FRAMEBUFFER,ne,i.RENDERBUFFER,Z)}}else{const K=R.texture.mipmaps;if(K&&K.length>0?t.bindFramebuffer(i.FRAMEBUFFER,x.__webglFramebuffer[0]):t.bindFramebuffer(i.FRAMEBUFFER,x.__webglFramebuffer),x.__webglDepthbuffer===void 0)x.__webglDepthbuffer=i.createRenderbuffer(),Ue(x.__webglDepthbuffer,R,!1);else{const ne=R.stencilBuffer?i.DEPTH_STENCIL_ATTACHMENT:i.DEPTH_ATTACHMENT,Z=x.__webglDepthbuffer;i.bindRenderbuffer(i.RENDERBUFFER,Z),i.framebufferRenderbuffer(i.FRAMEBUFFER,ne,i.RENDERBUFFER,Z)}}t.bindFramebuffer(i.FRAMEBUFFER,null)}function st(R,x,z){const K=n.get(R);x!==void 0&&Be(K.__webglFramebuffer,R,R.texture,i.COLOR_ATTACHMENT0,i.TEXTURE_2D,0),z!==void 0&&Mt(R)}function D(R){const x=R.texture,z=n.get(R),K=n.get(x);R.addEventListener("dispose",C);const ne=R.textures,Z=R.isWebGLCubeRenderTarget===!0,Pe=ne.length>1;if(Pe||(K.__webglTexture===void 0&&(K.__webglTexture=i.createTexture()),K.__version=x.version,a.memory.textures++),Z){z.__webglFramebuffer=[];for(let ce=0;ce<6;ce++)if(x.mipmaps&&x.mipmaps.length>0){z.__webglFramebuffer[ce]=[];for(let we=0;we<x.mipmaps.length;we++)z.__webglFramebuffer[ce][we]=i.createFramebuffer()}else z.__webglFramebuffer[ce]=i.createFramebuffer()}else{if(x.mipmaps&&x.mipmaps.length>0){z.__webglFramebuffer=[];for(let ce=0;ce<x.mipmaps.length;ce++)z.__webglFramebuffer[ce]=i.createFramebuffer()}else z.__webglFramebuffer=i.createFramebuffer();if(Pe)for(let ce=0,we=ne.length;ce<we;ce++){const Ae=n.get(ne[ce]);Ae.__webglTexture===void 0&&(Ae.__webglTexture=i.createTexture(),a.memory.textures++)}if(R.samples>0&&Se(R)===!1){z.__webglMultisampledFramebuffer=i.createFramebuffer(),z.__webglColorRenderbuffer=[],t.bindFramebuffer(i.FRAMEBUFFER,z.__webglMultisampledFramebuffer);for(let ce=0;ce<ne.length;ce++){const we=ne[ce];z.__webglColorRenderbuffer[ce]=i.createRenderbuffer(),i.bindRenderbuffer(i.RENDERBUFFER,z.__webglColorRenderbuffer[ce]);const Ae=r.convert(we.format,we.colorSpace),ae=r.convert(we.type),ge=b(we.internalFormat,Ae,ae,we.colorSpace,R.isXRRenderTarget===!0),Ge=yt(R);i.renderbufferStorageMultisample(i.RENDERBUFFER,Ge,ge,R.width,R.height),i.framebufferRenderbuffer(i.FRAMEBUFFER,i.COLOR_ATTACHMENT0+ce,i.RENDERBUFFER,z.__webglColorRenderbuffer[ce])}i.bindRenderbuffer(i.RENDERBUFFER,null),R.depthBuffer&&(z.__webglDepthRenderbuffer=i.createRenderbuffer(),Ue(z.__webglDepthRenderbuffer,R,!0)),t.bindFramebuffer(i.FRAMEBUFFER,null)}}if(Z){t.bindTexture(i.TEXTURE_CUBE_MAP,K.__webglTexture),Oe(i.TEXTURE_CUBE_MAP,x);for(let ce=0;ce<6;ce++)if(x.mipmaps&&x.mipmaps.length>0)for(let we=0;we<x.mipmaps.length;we++)Be(z.__webglFramebuffer[ce][we],R,x,i.COLOR_ATTACHMENT0,i.TEXTURE_CUBE_MAP_POSITIVE_X+ce,we);else Be(z.__webglFramebuffer[ce],R,x,i.COLOR_ATTACHMENT0,i.TEXTURE_CUBE_MAP_POSITIVE_X+ce,0);m(x)&&f(i.TEXTURE_CUBE_MAP),t.unbindTexture()}else if(Pe){for(let ce=0,we=ne.length;ce<we;ce++){const Ae=ne[ce],ae=n.get(Ae);let ge=i.TEXTURE_2D;(R.isWebGL3DRenderTarget||R.isWebGLArrayRenderTarget)&&(ge=R.isWebGL3DRenderTarget?i.TEXTURE_3D:i.TEXTURE_2D_ARRAY),t.bindTexture(ge,ae.__webglTexture),Oe(ge,Ae),Be(z.__webglFramebuffer,R,Ae,i.COLOR_ATTACHMENT0+ce,ge,0),m(Ae)&&f(ge)}t.unbindTexture()}else{let ce=i.TEXTURE_2D;if((R.isWebGL3DRenderTarget||R.isWebGLArrayRenderTarget)&&(ce=R.isWebGL3DRenderTarget?i.TEXTURE_3D:i.TEXTURE_2D_ARRAY),t.bindTexture(ce,K.__webglTexture),Oe(ce,x),x.mipmaps&&x.mipmaps.length>0)for(let we=0;we<x.mipmaps.length;we++)Be(z.__webglFramebuffer[we],R,x,i.COLOR_ATTACHMENT0,ce,we);else Be(z.__webglFramebuffer,R,x,i.COLOR_ATTACHMENT0,ce,0);m(x)&&f(ce),t.unbindTexture()}R.depthBuffer&&Mt(R)}function ft(R){const x=R.textures;for(let z=0,K=x.length;z<K;z++){const ne=x[z];if(m(ne)){const Z=E(R),Pe=n.get(ne).__webglTexture;t.bindTexture(Z,Pe),f(Z),t.unbindTexture()}}}const Ne=[],rt=[];function ke(R){if(R.samples>0){if(Se(R)===!1){const x=R.textures,z=R.width,K=R.height;let ne=i.COLOR_BUFFER_BIT;const Z=R.stencilBuffer?i.DEPTH_STENCIL_ATTACHMENT:i.DEPTH_ATTACHMENT,Pe=n.get(R),ce=x.length>1;if(ce)for(let Ae=0;Ae<x.length;Ae++)t.bindFramebuffer(i.FRAMEBUFFER,Pe.__webglMultisampledFramebuffer),i.framebufferRenderbuffer(i.FRAMEBUFFER,i.COLOR_ATTACHMENT0+Ae,i.RENDERBUFFER,null),t.bindFramebuffer(i.FRAMEBUFFER,Pe.__webglFramebuffer),i.framebufferTexture2D(i.DRAW_FRAMEBUFFER,i.COLOR_ATTACHMENT0+Ae,i.TEXTURE_2D,null,0);t.bindFramebuffer(i.READ_FRAMEBUFFER,Pe.__webglMultisampledFramebuffer);const we=R.texture.mipmaps;we&&we.length>0?t.bindFramebuffer(i.DRAW_FRAMEBUFFER,Pe.__webglFramebuffer[0]):t.bindFramebuffer(i.DRAW_FRAMEBUFFER,Pe.__webglFramebuffer);for(let Ae=0;Ae<x.length;Ae++){if(R.resolveDepthBuffer&&(R.depthBuffer&&(ne|=i.DEPTH_BUFFER_BIT),R.stencilBuffer&&R.resolveStencilBuffer&&(ne|=i.STENCIL_BUFFER_BIT)),ce){i.framebufferRenderbuffer(i.READ_FRAMEBUFFER,i.COLOR_ATTACHMENT0,i.RENDERBUFFER,Pe.__webglColorRenderbuffer[Ae]);const ae=n.get(x[Ae]).__webglTexture;i.framebufferTexture2D(i.DRAW_FRAMEBUFFER,i.COLOR_ATTACHMENT0,i.TEXTURE_2D,ae,0)}i.blitFramebuffer(0,0,z,K,0,0,z,K,ne,i.NEAREST),l===!0&&(Ne.length=0,rt.length=0,Ne.push(i.COLOR_ATTACHMENT0+Ae),R.depthBuffer&&R.resolveDepthBuffer===!1&&(Ne.push(Z),rt.push(Z),i.invalidateFramebuffer(i.DRAW_FRAMEBUFFER,rt)),i.invalidateFramebuffer(i.READ_FRAMEBUFFER,Ne))}if(t.bindFramebuffer(i.READ_FRAMEBUFFER,null),t.bindFramebuffer(i.DRAW_FRAMEBUFFER,null),ce)for(let Ae=0;Ae<x.length;Ae++){t.bindFramebuffer(i.FRAMEBUFFER,Pe.__webglMultisampledFramebuffer),i.framebufferRenderbuffer(i.FRAMEBUFFER,i.COLOR_ATTACHMENT0+Ae,i.RENDERBUFFER,Pe.__webglColorRenderbuffer[Ae]);const ae=n.get(x[Ae]).__webglTexture;t.bindFramebuffer(i.FRAMEBUFFER,Pe.__webglFramebuffer),i.framebufferTexture2D(i.DRAW_FRAMEBUFFER,i.COLOR_ATTACHMENT0+Ae,i.TEXTURE_2D,ae,0)}t.bindFramebuffer(i.DRAW_FRAMEBUFFER,Pe.__webglMultisampledFramebuffer)}else if(R.depthBuffer&&R.resolveDepthBuffer===!1&&l){const x=R.stencilBuffer?i.DEPTH_STENCIL_ATTACHMENT:i.DEPTH_ATTACHMENT;i.invalidateFramebuffer(i.DRAW_FRAMEBUFFER,[x])}}}function yt(R){return Math.min(s.maxSamples,R.samples)}function Se(R){const x=n.get(R);return R.samples>0&&e.has("WEBGL_multisampled_render_to_texture")===!0&&x.__useRenderToTexture!==!1}function Je(R){const x=a.render.frame;u.get(R)!==x&&(u.set(R,x),R.update())}function Dt(R,x){const z=R.colorSpace,K=R.format,ne=R.type;return R.isCompressedTexture===!0||R.isVideoTexture===!0||z!==Hi&&z!==Hn&&(lt.getTransfer(z)===mt?(K!==hn||ne!==Mn)&&console.warn("THREE.WebGLTextures: sRGB encoded textures have to use RGBAFormat and UnsignedByteType."):console.error("THREE.WebGLTextures: Unsupported texture color space:",z)),x}function Rt(R){return typeof HTMLImageElement<"u"&&R instanceof HTMLImageElement?(c.width=R.naturalWidth||R.width,c.height=R.naturalHeight||R.height):typeof VideoFrame<"u"&&R instanceof VideoFrame?(c.width=R.displayWidth,c.height=R.displayHeight):(c.width=R.width,c.height=R.height),c}this.allocateTextureUnit=q,this.resetTextureUnits=$,this.setTexture2D=X,this.setTexture2DArray=B,this.setTexture3D=Q,this.setTextureCube=G,this.rebindTextures=st,this.setupRenderTarget=D,this.updateRenderTargetMipmap=ft,this.updateMultisampleRenderTarget=ke,this.setupDepthRenderbuffer=Mt,this.setupFrameBufferTexture=Be,this.useMultisampledRTT=Se}function bv(i,e){function t(n,s=Hn){let r;const a=lt.getTransfer(s);if(n===Mn)return i.UNSIGNED_BYTE;if(n===Mo)return i.UNSIGNED_SHORT_4_4_4_4;if(n===So)return i.UNSIGNED_SHORT_5_5_5_1;if(n===bc)return i.UNSIGNED_INT_5_9_9_9_REV;if(n===Sc)return i.BYTE;if(n===yc)return i.SHORT;if(n===as)return i.UNSIGNED_SHORT;if(n===xo)return i.INT;if(n===ui)return i.UNSIGNED_INT;if(n===Pn)return i.FLOAT;if(n===ps)return i.HALF_FLOAT;if(n===Ec)return i.ALPHA;if(n===Tc)return i.RGB;if(n===hn)return i.RGBA;if(n===ls)return i.DEPTH_COMPONENT;if(n===cs)return i.DEPTH_STENCIL;if(n===wc)return i.RED;if(n===yo)return i.RED_INTEGER;if(n===Ac)return i.RG;if(n===bo)return i.RG_INTEGER;if(n===Eo)return i.RGBA_INTEGER;if(n===js||n===Zs||n===Ks||n===Js)if(a===mt)if(r=e.get("WEBGL_compressed_texture_s3tc_srgb"),r!==null){if(n===js)return r.COMPRESSED_SRGB_S3TC_DXT1_EXT;if(n===Zs)return r.COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;if(n===Ks)return r.COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;if(n===Js)return r.COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT}else return null;else if(r=e.get("WEBGL_compressed_texture_s3tc"),r!==null){if(n===js)return r.COMPRESSED_RGB_S3TC_DXT1_EXT;if(n===Zs)return r.COMPRESSED_RGBA_S3TC_DXT1_EXT;if(n===Ks)return r.COMPRESSED_RGBA_S3TC_DXT3_EXT;if(n===Js)return r.COMPRESSED_RGBA_S3TC_DXT5_EXT}else return null;if(n===La||n===Da||n===Ia||n===Ua)if(r=e.get("WEBGL_compressed_texture_pvrtc"),r!==null){if(n===La)return r.COMPRESSED_RGB_PVRTC_4BPPV1_IMG;if(n===Da)return r.COMPRESSED_RGB_PVRTC_2BPPV1_IMG;if(n===Ia)return r.COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;if(n===Ua)return r.COMPRESSED_RGBA_PVRTC_2BPPV1_IMG}else return null;if(n===ka||n===Na||n===Fa)if(r=e.get("WEBGL_compressed_texture_etc"),r!==null){if(n===ka||n===Na)return a===mt?r.COMPRESSED_SRGB8_ETC2:r.COMPRESSED_RGB8_ETC2;if(n===Fa)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:r.COMPRESSED_RGBA8_ETC2_EAC}else return null;if(n===Oa||n===Ba||n===za||n===Va||n===Ha||n===Ga||n===Wa||n===$a||n===Xa||n===qa||n===Ya||n===ja||n===Za||n===Ka)if(r=e.get("WEBGL_compressed_texture_astc"),r!==null){if(n===Oa)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:r.COMPRESSED_RGBA_ASTC_4x4_KHR;if(n===Ba)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:r.COMPRESSED_RGBA_ASTC_5x4_KHR;if(n===za)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:r.COMPRESSED_RGBA_ASTC_5x5_KHR;if(n===Va)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:r.COMPRESSED_RGBA_ASTC_6x5_KHR;if(n===Ha)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:r.COMPRESSED_RGBA_ASTC_6x6_KHR;if(n===Ga)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:r.COMPRESSED_RGBA_ASTC_8x5_KHR;if(n===Wa)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:r.COMPRESSED_RGBA_ASTC_8x6_KHR;if(n===$a)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:r.COMPRESSED_RGBA_ASTC_8x8_KHR;if(n===Xa)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:r.COMPRESSED_RGBA_ASTC_10x5_KHR;if(n===qa)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:r.COMPRESSED_RGBA_ASTC_10x6_KHR;if(n===Ya)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:r.COMPRESSED_RGBA_ASTC_10x8_KHR;if(n===ja)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:r.COMPRESSED_RGBA_ASTC_10x10_KHR;if(n===Za)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:r.COMPRESSED_RGBA_ASTC_12x10_KHR;if(n===Ka)return a===mt?r.COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:r.COMPRESSED_RGBA_ASTC_12x12_KHR}else return null;if(n===Qs||n===Ja||n===Qa)if(r=e.get("EXT_texture_compression_bptc"),r!==null){if(n===Qs)return a===mt?r.COMPRESSED_SRGB_ALPHA_BPTC_UNORM_EXT:r.COMPRESSED_RGBA_BPTC_UNORM_EXT;if(n===Ja)return r.COMPRESSED_RGB_BPTC_SIGNED_FLOAT_EXT;if(n===Qa)return r.COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_EXT}else return null;if(n===Rc||n===eo||n===to||n===no)if(r=e.get("EXT_texture_compression_rgtc"),r!==null){if(n===Qs)return r.COMPRESSED_RED_RGTC1_EXT;if(n===eo)return r.COMPRESSED_SIGNED_RED_RGTC1_EXT;if(n===to)return r.COMPRESSED_RED_GREEN_RGTC2_EXT;if(n===no)return r.COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT}else return null;return n===os?i.UNSIGNED_INT_24_8:i[n]!==void 0?i[n]:null}return{convert:t}}class jc extends Bt{constructor(e=null){super(),this.sourceTexture=e,this.isExternalTexture=!0}}const Ev=`
void main() {

	gl_Position = vec4( position, 1.0 );

}`,Tv=`
uniform sampler2DArray depthColor;
uniform float depthWidth;
uniform float depthHeight;

void main() {

	vec2 coord = vec2( gl_FragCoord.x / depthWidth, gl_FragCoord.y / depthHeight );

	if ( coord.x >= 1.0 ) {

		gl_FragDepth = texture( depthColor, vec3( coord.x - 1.0, coord.y, 1 ) ).r;

	} else {

		gl_FragDepth = texture( depthColor, vec3( coord.x, coord.y, 0 ) ).r;

	}

}`;class wv{constructor(){this.texture=null,this.mesh=null,this.depthNear=0,this.depthFar=0}init(e,t){if(this.texture===null){const n=new jc(e.texture);(e.depthNear!==t.depthNear||e.depthFar!==t.depthFar)&&(this.depthNear=e.depthNear,this.depthFar=e.depthFar),this.texture=n}}getMesh(e){if(this.texture!==null&&this.mesh===null){const t=e.cameras[0].viewport,n=new on({vertexShader:Ev,fragmentShader:Tv,uniforms:{depthColor:{value:this.texture},depthWidth:{value:t.z},depthHeight:{value:t.w}}});this.mesh=new kt(new Yn(20,20),n)}return this.mesh}reset(){this.texture=null,this.mesh=null}getDepthTexture(){return this.texture}}class Av extends Wi{constructor(e,t){super();const n=this;let s=null,r=1,a=null,o="local-floor",l=1,c=null,u=null,d=null,h=null,p=null,g=null;const _=new wv,m={},f=t.getContextAttributes();let E=null,b=null;const v=[],T=[],A=new et;let C=null;const I=new Wt;I.viewport=new At;const y=new Wt;y.viewport=new At;const S=[I,y],P=new qf;let $=null,q=null;this.cameraAutoUpdate=!0,this.enabled=!1,this.isPresenting=!1,this.getController=function(j){let he=v[j];return he===void 0&&(he=new Zr,v[j]=he),he.getTargetRaySpace()},this.getControllerGrip=function(j){let he=v[j];return he===void 0&&(he=new Zr,v[j]=he),he.getGripSpace()},this.getHand=function(j){let he=v[j];return he===void 0&&(he=new Zr,v[j]=he),he.getHandSpace()};function Y(j){const he=T.indexOf(j.inputSource);if(he===-1)return;const le=v[he];le!==void 0&&(le.update(j.inputSource,j.frame,c||a),le.dispatchEvent({type:j.type,data:j.inputSource}))}function X(){s.removeEventListener("select",Y),s.removeEventListener("selectstart",Y),s.removeEventListener("selectend",Y),s.removeEventListener("squeeze",Y),s.removeEventListener("squeezestart",Y),s.removeEventListener("squeezeend",Y),s.removeEventListener("end",X),s.removeEventListener("inputsourceschange",B);for(let j=0;j<v.length;j++){const he=T[j];he!==null&&(T[j]=null,v[j].disconnect(he))}$=null,q=null,_.reset();for(const j in m)delete m[j];e.setRenderTarget(E),p=null,h=null,d=null,s=null,b=null,He.stop(),n.isPresenting=!1,e.setPixelRatio(C),e.setSize(A.width,A.height,!1),n.dispatchEvent({type:"sessionend"})}this.setFramebufferScaleFactor=function(j){r=j,n.isPresenting===!0&&console.warn("THREE.WebXRManager: Cannot change framebuffer scale while presenting.")},this.setReferenceSpaceType=function(j){o=j,n.isPresenting===!0&&console.warn("THREE.WebXRManager: Cannot change reference space type while presenting.")},this.getReferenceSpace=function(){return c||a},this.setReferenceSpace=function(j){c=j},this.getBaseLayer=function(){return h!==null?h:p},this.getBinding=function(){return d},this.getFrame=function(){return g},this.getSession=function(){return s},this.setSession=async function(j){if(s=j,s!==null){if(E=e.getRenderTarget(),s.addEventListener("select",Y),s.addEventListener("selectstart",Y),s.addEventListener("selectend",Y),s.addEventListener("squeeze",Y),s.addEventListener("squeezestart",Y),s.addEventListener("squeezeend",Y),s.addEventListener("end",X),s.addEventListener("inputsourceschange",B),f.xrCompatible!==!0&&await t.makeXRCompatible(),C=e.getPixelRatio(),e.getSize(A),typeof XRWebGLBinding<"u"&&(d=new XRWebGLBinding(s,t)),d!==null&&"createProjectionLayer"in XRWebGLBinding.prototype){let le=null,Be=null,Ue=null;f.depth&&(Ue=f.stencil?t.DEPTH24_STENCIL8:t.DEPTH_COMPONENT24,le=f.stencil?cs:ls,Be=f.stencil?os:ui);const $e={colorFormat:t.RGBA8,depthFormat:Ue,scaleFactor:r};h=d.createProjectionLayer($e),s.updateRenderState({layers:[h]}),e.setPixelRatio(1),e.setSize(h.textureWidth,h.textureHeight,!1),b=new di(h.textureWidth,h.textureHeight,{format:hn,type:Mn,depthTexture:new Vc(h.textureWidth,h.textureHeight,Be,void 0,void 0,void 0,void 0,void 0,void 0,le),stencilBuffer:f.stencil,colorSpace:e.outputColorSpace,samples:f.antialias?4:0,resolveDepthBuffer:h.ignoreDepthValues===!1,resolveStencilBuffer:h.ignoreDepthValues===!1})}else{const le={antialias:f.antialias,alpha:!0,depth:f.depth,stencil:f.stencil,framebufferScaleFactor:r};p=new XRWebGLLayer(s,t,le),s.updateRenderState({baseLayer:p}),e.setPixelRatio(1),e.setSize(p.framebufferWidth,p.framebufferHeight,!1),b=new di(p.framebufferWidth,p.framebufferHeight,{format:hn,type:Mn,colorSpace:e.outputColorSpace,stencilBuffer:f.stencil,resolveDepthBuffer:p.ignoreDepthValues===!1,resolveStencilBuffer:p.ignoreDepthValues===!1})}b.isXRRenderTarget=!0,this.setFoveation(l),c=null,a=await s.requestReferenceSpace(o),He.setContext(s),He.start(),n.isPresenting=!0,n.dispatchEvent({type:"sessionstart"})}},this.getEnvironmentBlendMode=function(){if(s!==null)return s.environmentBlendMode},this.getDepthTexture=function(){return _.getDepthTexture()};function B(j){for(let he=0;he<j.removed.length;he++){const le=j.removed[he],Be=T.indexOf(le);Be>=0&&(T[Be]=null,v[Be].disconnect(le))}for(let he=0;he<j.added.length;he++){const le=j.added[he];let Be=T.indexOf(le);if(Be===-1){for(let $e=0;$e<v.length;$e++)if($e>=T.length){T.push(le),Be=$e;break}else if(T[$e]===null){T[$e]=le,Be=$e;break}if(Be===-1)break}const Ue=v[Be];Ue&&Ue.connect(le)}}const Q=new O,G=new O;function ie(j,he,le){Q.setFromMatrixPosition(he.matrixWorld),G.setFromMatrixPosition(le.matrixWorld);const Be=Q.distanceTo(G),Ue=he.projectionMatrix.elements,$e=le.projectionMatrix.elements,Mt=Ue[14]/(Ue[10]-1),st=Ue[14]/(Ue[10]+1),D=(Ue[9]+1)/Ue[5],ft=(Ue[9]-1)/Ue[5],Ne=(Ue[8]-1)/Ue[0],rt=($e[8]+1)/$e[0],ke=Mt*Ne,yt=Mt*rt,Se=Be/(-Ne+rt),Je=Se*-Ne;if(he.matrixWorld.decompose(j.position,j.quaternion,j.scale),j.translateX(Je),j.translateZ(Se),j.matrixWorld.compose(j.position,j.quaternion,j.scale),j.matrixWorldInverse.copy(j.matrixWorld).invert(),Ue[10]===-1)j.projectionMatrix.copy(he.projectionMatrix),j.projectionMatrixInverse.copy(he.projectionMatrixInverse);else{const Dt=Mt+Se,Rt=st+Se,R=ke-Je,x=yt+(Be-Je),z=D*st/Rt*Dt,K=ft*st/Rt*Dt;j.projectionMatrix.makePerspective(R,x,z,K,Dt,Rt),j.projectionMatrixInverse.copy(j.projectionMatrix).invert()}}function de(j,he){he===null?j.matrixWorld.copy(j.matrix):j.matrixWorld.multiplyMatrices(he.matrixWorld,j.matrix),j.matrixWorldInverse.copy(j.matrixWorld).invert()}this.updateCamera=function(j){if(s===null)return;let he=j.near,le=j.far;_.texture!==null&&(_.depthNear>0&&(he=_.depthNear),_.depthFar>0&&(le=_.depthFar)),P.near=y.near=I.near=he,P.far=y.far=I.far=le,($!==P.near||q!==P.far)&&(s.updateRenderState({depthNear:P.near,depthFar:P.far}),$=P.near,q=P.far),P.layers.mask=j.layers.mask|6,I.layers.mask=P.layers.mask&3,y.layers.mask=P.layers.mask&5;const Be=j.parent,Ue=P.cameras;de(P,Be);for(let $e=0;$e<Ue.length;$e++)de(Ue[$e],Be);Ue.length===2?ie(P,I,y):P.projectionMatrix.copy(I.projectionMatrix),Ee(j,P,Be)};function Ee(j,he,le){le===null?j.matrix.copy(he.matrixWorld):(j.matrix.copy(le.matrixWorld),j.matrix.invert(),j.matrix.multiply(he.matrixWorld)),j.matrix.decompose(j.position,j.quaternion,j.scale),j.updateMatrixWorld(!0),j.projectionMatrix.copy(he.projectionMatrix),j.projectionMatrixInverse.copy(he.projectionMatrixInverse),j.isPerspectiveCamera&&(j.fov=us*2*Math.atan(1/j.projectionMatrix.elements[5]),j.zoom=1)}this.getCamera=function(){return P},this.getFoveation=function(){if(!(h===null&&p===null))return l},this.setFoveation=function(j){l=j,h!==null&&(h.fixedFoveation=j),p!==null&&p.fixedFoveation!==void 0&&(p.fixedFoveation=j)},this.hasDepthSensing=function(){return _.texture!==null},this.getDepthSensingMesh=function(){return _.getMesh(P)},this.getCameraTexture=function(j){return m[j]};let Oe=null;function at(j,he){if(u=he.getViewerPose(c||a),g=he,u!==null){const le=u.views;p!==null&&(e.setRenderTargetFramebuffer(b,p.framebuffer),e.setRenderTarget(b));let Be=!1;le.length!==P.cameras.length&&(P.cameras.length=0,Be=!0);for(let st=0;st<le.length;st++){const D=le[st];let ft=null;if(p!==null)ft=p.getViewport(D);else{const rt=d.getViewSubImage(h,D);ft=rt.viewport,st===0&&(e.setRenderTargetTextures(b,rt.colorTexture,rt.depthStencilTexture),e.setRenderTarget(b))}let Ne=S[st];Ne===void 0&&(Ne=new Wt,Ne.layers.enable(st),Ne.viewport=new At,S[st]=Ne),Ne.matrix.fromArray(D.transform.matrix),Ne.matrix.decompose(Ne.position,Ne.quaternion,Ne.scale),Ne.projectionMatrix.fromArray(D.projectionMatrix),Ne.projectionMatrixInverse.copy(Ne.projectionMatrix).invert(),Ne.viewport.set(ft.x,ft.y,ft.width,ft.height),st===0&&(P.matrix.copy(Ne.matrix),P.matrix.decompose(P.position,P.quaternion,P.scale)),Be===!0&&P.cameras.push(Ne)}const Ue=s.enabledFeatures;if(Ue&&Ue.includes("depth-sensing")&&s.depthUsage=="gpu-optimized"&&d){const st=d.getDepthInformation(le[0]);st&&st.isValid&&st.texture&&_.init(st,s.renderState)}if(Ue&&Ue.includes("camera-access")&&(e.state.unbindTexture(),d))for(let st=0;st<le.length;st++){const D=le[st].camera;if(D){let ft=m[D];ft||(ft=new jc,m[D]=ft);const Ne=d.getCameraImage(D);ft.sourceTexture=Ne}}}for(let le=0;le<v.length;le++){const Be=T[le],Ue=v[le];Be!==null&&Ue!==void 0&&Ue.update(Be,he,c||a)}Oe&&Oe(j,he),he.detectedPlanes&&n.dispatchEvent({type:"planesdetected",data:he}),g=null}const He=new Wc;He.setAnimationLoop(at),this.setAnimationLoop=function(j){Oe=j},this.dispose=function(){}}}const ii=new Sn,Rv=new bt;function Cv(i,e){function t(m,f){m.matrixAutoUpdate===!0&&m.updateMatrix(),f.value.copy(m.matrix)}function n(m,f){f.color.getRGB(m.fogColor.value,Nc(i)),f.isFog?(m.fogNear.value=f.near,m.fogFar.value=f.far):f.isFogExp2&&(m.fogDensity.value=f.density)}function s(m,f,E,b,v){f.isMeshBasicMaterial||f.isMeshLambertMaterial?r(m,f):f.isMeshToonMaterial?(r(m,f),d(m,f)):f.isMeshPhongMaterial?(r(m,f),u(m,f)):f.isMeshStandardMaterial?(r(m,f),h(m,f),f.isMeshPhysicalMaterial&&p(m,f,v)):f.isMeshMatcapMaterial?(r(m,f),g(m,f)):f.isMeshDepthMaterial?r(m,f):f.isMeshDistanceMaterial?(r(m,f),_(m,f)):f.isMeshNormalMaterial?r(m,f):f.isLineBasicMaterial?(a(m,f),f.isLineDashedMaterial&&o(m,f)):f.isPointsMaterial?l(m,f,E,b):f.isSpriteMaterial?c(m,f):f.isShadowMaterial?(m.color.value.copy(f.color),m.opacity.value=f.opacity):f.isShaderMaterial&&(f.uniformsNeedUpdate=!1)}function r(m,f){m.opacity.value=f.opacity,f.color&&m.diffuse.value.copy(f.color),f.emissive&&m.emissive.value.copy(f.emissive).multiplyScalar(f.emissiveIntensity),f.map&&(m.map.value=f.map,t(f.map,m.mapTransform)),f.alphaMap&&(m.alphaMap.value=f.alphaMap,t(f.alphaMap,m.alphaMapTransform)),f.bumpMap&&(m.bumpMap.value=f.bumpMap,t(f.bumpMap,m.bumpMapTransform),m.bumpScale.value=f.bumpScale,f.side===Yt&&(m.bumpScale.value*=-1)),f.normalMap&&(m.normalMap.value=f.normalMap,t(f.normalMap,m.normalMapTransform),m.normalScale.value.copy(f.normalScale),f.side===Yt&&m.normalScale.value.negate()),f.displacementMap&&(m.displacementMap.value=f.displacementMap,t(f.displacementMap,m.displacementMapTransform),m.displacementScale.value=f.displacementScale,m.displacementBias.value=f.displacementBias),f.emissiveMap&&(m.emissiveMap.value=f.emissiveMap,t(f.emissiveMap,m.emissiveMapTransform)),f.specularMap&&(m.specularMap.value=f.specularMap,t(f.specularMap,m.specularMapTransform)),f.alphaTest>0&&(m.alphaTest.value=f.alphaTest);const E=e.get(f),b=E.envMap,v=E.envMapRotation;b&&(m.envMap.value=b,ii.copy(v),ii.x*=-1,ii.y*=-1,ii.z*=-1,b.isCubeTexture&&b.isRenderTargetTexture===!1&&(ii.y*=-1,ii.z*=-1),m.envMapRotation.value.setFromMatrix4(Rv.makeRotationFromEuler(ii)),m.flipEnvMap.value=b.isCubeTexture&&b.isRenderTargetTexture===!1?-1:1,m.reflectivity.value=f.reflectivity,m.ior.value=f.ior,m.refractionRatio.value=f.refractionRatio),f.lightMap&&(m.lightMap.value=f.lightMap,m.lightMapIntensity.value=f.lightMapIntensity,t(f.lightMap,m.lightMapTransform)),f.aoMap&&(m.aoMap.value=f.aoMap,m.aoMapIntensity.value=f.aoMapIntensity,t(f.aoMap,m.aoMapTransform))}function a(m,f){m.diffuse.value.copy(f.color),m.opacity.value=f.opacity,f.map&&(m.map.value=f.map,t(f.map,m.mapTransform))}function o(m,f){m.dashSize.value=f.dashSize,m.totalSize.value=f.dashSize+f.gapSize,m.scale.value=f.scale}function l(m,f,E,b){m.diffuse.value.copy(f.color),m.opacity.value=f.opacity,m.size.value=f.size*E,m.scale.value=b*.5,f.map&&(m.map.value=f.map,t(f.map,m.uvTransform)),f.alphaMap&&(m.alphaMap.value=f.alphaMap,t(f.alphaMap,m.alphaMapTransform)),f.alphaTest>0&&(m.alphaTest.value=f.alphaTest)}function c(m,f){m.diffuse.value.copy(f.color),m.opacity.value=f.opacity,m.rotation.value=f.rotation,f.map&&(m.map.value=f.map,t(f.map,m.mapTransform)),f.alphaMap&&(m.alphaMap.value=f.alphaMap,t(f.alphaMap,m.alphaMapTransform)),f.alphaTest>0&&(m.alphaTest.value=f.alphaTest)}function u(m,f){m.specular.value.copy(f.specular),m.shininess.value=Math.max(f.shininess,1e-4)}function d(m,f){f.gradientMap&&(m.gradientMap.value=f.gradientMap)}function h(m,f){m.metalness.value=f.metalness,f.metalnessMap&&(m.metalnessMap.value=f.metalnessMap,t(f.metalnessMap,m.metalnessMapTransform)),m.roughness.value=f.roughness,f.roughnessMap&&(m.roughnessMap.value=f.roughnessMap,t(f.roughnessMap,m.roughnessMapTransform)),f.envMap&&(m.envMapIntensity.value=f.envMapIntensity)}function p(m,f,E){m.ior.value=f.ior,f.sheen>0&&(m.sheenColor.value.copy(f.sheenColor).multiplyScalar(f.sheen),m.sheenRoughness.value=f.sheenRoughness,f.sheenColorMap&&(m.sheenColorMap.value=f.sheenColorMap,t(f.sheenColorMap,m.sheenColorMapTransform)),f.sheenRoughnessMap&&(m.sheenRoughnessMap.value=f.sheenRoughnessMap,t(f.sheenRoughnessMap,m.sheenRoughnessMapTransform))),f.clearcoat>0&&(m.clearcoat.value=f.clearcoat,m.clearcoatRoughness.value=f.clearcoatRoughness,f.clearcoatMap&&(m.clearcoatMap.value=f.clearcoatMap,t(f.clearcoatMap,m.clearcoatMapTransform)),f.clearcoatRoughnessMap&&(m.clearcoatRoughnessMap.value=f.clearcoatRoughnessMap,t(f.clearcoatRoughnessMap,m.clearcoatRoughnessMapTransform)),f.clearcoatNormalMap&&(m.clearcoatNormalMap.value=f.clearcoatNormalMap,t(f.clearcoatNormalMap,m.clearcoatNormalMapTransform),m.clearcoatNormalScale.value.copy(f.clearcoatNormalScale),f.side===Yt&&m.clearcoatNormalScale.value.negate())),f.dispersion>0&&(m.dispersion.value=f.dispersion),f.iridescence>0&&(m.iridescence.value=f.iridescence,m.iridescenceIOR.value=f.iridescenceIOR,m.iridescenceThicknessMinimum.value=f.iridescenceThicknessRange[0],m.iridescenceThicknessMaximum.value=f.iridescenceThicknessRange[1],f.iridescenceMap&&(m.iridescenceMap.value=f.iridescenceMap,t(f.iridescenceMap,m.iridescenceMapTransform)),f.iridescenceThicknessMap&&(m.iridescenceThicknessMap.value=f.iridescenceThicknessMap,t(f.iridescenceThicknessMap,m.iridescenceThicknessMapTransform))),f.transmission>0&&(m.transmission.value=f.transmission,m.transmissionSamplerMap.value=E.texture,m.transmissionSamplerSize.value.set(E.width,E.height),f.transmissionMap&&(m.transmissionMap.value=f.transmissionMap,t(f.transmissionMap,m.transmissionMapTransform)),m.thickness.value=f.thickness,f.thicknessMap&&(m.thicknessMap.value=f.thicknessMap,t(f.thicknessMap,m.thicknessMapTransform)),m.attenuationDistance.value=f.attenuationDistance,m.attenuationColor.value.copy(f.attenuationColor)),f.anisotropy>0&&(m.anisotropyVector.value.set(f.anisotropy*Math.cos(f.anisotropyRotation),f.anisotropy*Math.sin(f.anisotropyRotation)),f.anisotropyMap&&(m.anisotropyMap.value=f.anisotropyMap,t(f.anisotropyMap,m.anisotropyMapTransform))),m.specularIntensity.value=f.specularIntensity,m.specularColor.value.copy(f.specularColor),f.specularColorMap&&(m.specularColorMap.value=f.specularColorMap,t(f.specularColorMap,m.specularColorMapTransform)),f.specularIntensityMap&&(m.specularIntensityMap.value=f.specularIntensityMap,t(f.specularIntensityMap,m.specularIntensityMapTransform))}function g(m,f){f.matcap&&(m.matcap.value=f.matcap)}function _(m,f){const E=e.get(f).light;m.referencePosition.value.setFromMatrixPosition(E.matrixWorld),m.nearDistance.value=E.shadow.camera.near,m.farDistance.value=E.shadow.camera.far}return{refreshFogUniforms:n,refreshMaterialUniforms:s}}function Pv(i,e,t,n){let s={},r={},a=[];const o=i.getParameter(i.MAX_UNIFORM_BUFFER_BINDINGS);function l(E,b){const v=b.program;n.uniformBlockBinding(E,v)}function c(E,b){let v=s[E.id];v===void 0&&(g(E),v=u(E),s[E.id]=v,E.addEventListener("dispose",m));const T=b.program;n.updateUBOMapping(E,T);const A=e.render.frame;r[E.id]!==A&&(h(E),r[E.id]=A)}function u(E){const b=d();E.__bindingPointIndex=b;const v=i.createBuffer(),T=E.__size,A=E.usage;return i.bindBuffer(i.UNIFORM_BUFFER,v),i.bufferData(i.UNIFORM_BUFFER,T,A),i.bindBuffer(i.UNIFORM_BUFFER,null),i.bindBufferBase(i.UNIFORM_BUFFER,b,v),v}function d(){for(let E=0;E<o;E++)if(a.indexOf(E)===-1)return a.push(E),E;return console.error("THREE.WebGLRenderer: Maximum number of simultaneously usable uniforms groups reached."),0}function h(E){const b=s[E.id],v=E.uniforms,T=E.__cache;i.bindBuffer(i.UNIFORM_BUFFER,b);for(let A=0,C=v.length;A<C;A++){const I=Array.isArray(v[A])?v[A]:[v[A]];for(let y=0,S=I.length;y<S;y++){const P=I[y];if(p(P,A,y,T)===!0){const $=P.__offset,q=Array.isArray(P.value)?P.value:[P.value];let Y=0;for(let X=0;X<q.length;X++){const B=q[X],Q=_(B);typeof B=="number"||typeof B=="boolean"?(P.__data[0]=B,i.bufferSubData(i.UNIFORM_BUFFER,$+Y,P.__data)):B.isMatrix3?(P.__data[0]=B.elements[0],P.__data[1]=B.elements[1],P.__data[2]=B.elements[2],P.__data[3]=0,P.__data[4]=B.elements[3],P.__data[5]=B.elements[4],P.__data[6]=B.elements[5],P.__data[7]=0,P.__data[8]=B.elements[6],P.__data[9]=B.elements[7],P.__data[10]=B.elements[8],P.__data[11]=0):(B.toArray(P.__data,Y),Y+=Q.storage/Float32Array.BYTES_PER_ELEMENT)}i.bufferSubData(i.UNIFORM_BUFFER,$,P.__data)}}}i.bindBuffer(i.UNIFORM_BUFFER,null)}function p(E,b,v,T){const A=E.value,C=b+"_"+v;if(T[C]===void 0)return typeof A=="number"||typeof A=="boolean"?T[C]=A:T[C]=A.clone(),!0;{const I=T[C];if(typeof A=="number"||typeof A=="boolean"){if(I!==A)return T[C]=A,!0}else if(I.equals(A)===!1)return I.copy(A),!0}return!1}function g(E){const b=E.uniforms;let v=0;const T=16;for(let C=0,I=b.length;C<I;C++){const y=Array.isArray(b[C])?b[C]:[b[C]];for(let S=0,P=y.length;S<P;S++){const $=y[S],q=Array.isArray($.value)?$.value:[$.value];for(let Y=0,X=q.length;Y<X;Y++){const B=q[Y],Q=_(B),G=v%T,ie=G%Q.boundary,de=G+ie;v+=ie,de!==0&&T-de<Q.storage&&(v+=T-de),$.__data=new Float32Array(Q.storage/Float32Array.BYTES_PER_ELEMENT),$.__offset=v,v+=Q.storage}}}const A=v%T;return A>0&&(v+=T-A),E.__size=v,E.__cache={},this}function _(E){const b={boundary:0,storage:0};return typeof E=="number"||typeof E=="boolean"?(b.boundary=4,b.storage=4):E.isVector2?(b.boundary=8,b.storage=8):E.isVector3||E.isColor?(b.boundary=16,b.storage=12):E.isVector4?(b.boundary=16,b.storage=16):E.isMatrix3?(b.boundary=48,b.storage=48):E.isMatrix4?(b.boundary=64,b.storage=64):E.isTexture?console.warn("THREE.WebGLRenderer: Texture samplers can not be part of an uniforms group."):console.warn("THREE.WebGLRenderer: Unsupported uniform value type.",E),b}function m(E){const b=E.target;b.removeEventListener("dispose",m);const v=a.indexOf(b.__bindingPointIndex);a.splice(v,1),i.deleteBuffer(s[b.id]),delete s[b.id],delete r[b.id]}function f(){for(const E in s)i.deleteBuffer(s[E]);a=[],s={},r={}}return{bind:l,update:c,dispose:f}}class yr{constructor(e={}){const{canvas:t=af(),context:n=null,depth:s=!0,stencil:r=!1,alpha:a=!1,antialias:o=!1,premultipliedAlpha:l=!0,preserveDrawingBuffer:c=!1,powerPreference:u="default",failIfMajorPerformanceCaveat:d=!1,reversedDepthBuffer:h=!1}=e;this.isWebGLRenderer=!0;let p;if(n!==null){if(typeof WebGLRenderingContext<"u"&&n instanceof WebGLRenderingContext)throw new Error("THREE.WebGLRenderer: WebGL 1 is not supported since r163.");p=n.getContextAttributes().alpha}else p=a;const g=new Uint32Array(4),_=new Int32Array(4);let m=null,f=null;const E=[],b=[];this.domElement=t,this.debug={checkShaderErrors:!0,onShaderError:null},this.autoClear=!0,this.autoClearColor=!0,this.autoClearDepth=!0,this.autoClearStencil=!0,this.sortObjects=!0,this.clippingPlanes=[],this.localClippingEnabled=!1,this.toneMapping=$n,this.toneMappingExposure=1,this.transmissionResolutionScale=1;const v=this;let T=!1;this._outputColorSpace=rn;let A=0,C=0,I=null,y=-1,S=null;const P=new At,$=new At;let q=null;const Y=new Le(0);let X=0,B=t.width,Q=t.height,G=1,ie=null,de=null;const Ee=new At(0,0,B,Q),Oe=new At(0,0,B,Q);let at=!1;const He=new Ro;let j=!1,he=!1;const le=new bt,Be=new O,Ue=new At,$e={background:null,fog:null,environment:null,overrideMaterial:null,isScene:!0};let Mt=!1;function st(){return I===null?G:1}let D=n;function ft(M,k){return t.getContext(M,k)}try{const M={alpha:!0,depth:s,stencil:r,antialias:o,premultipliedAlpha:l,preserveDrawingBuffer:c,powerPreference:u,failIfMajorPerformanceCaveat:d};if("setAttribute"in t&&t.setAttribute("data-engine",`three.js r${go}`),t.addEventListener("webglcontextlost",ue,!1),t.addEventListener("webglcontextrestored",Me,!1),t.addEventListener("webglcontextcreationerror",se,!1),D===null){const k="webgl2";if(D=ft(k,M),D===null)throw ft(k)?new Error("Error creating WebGL context with your selected attributes."):new Error("Error creating WebGL context.")}}catch(M){throw console.error("THREE.WebGLRenderer: "+M.message),M}let Ne,rt,ke,yt,Se,Je,Dt,Rt,R,x,z,K,ne,Z,Pe,ce,we,Ae,ae,ge,Ge,Re,pe,je;function U(){Ne=new Vg(D),Ne.init(),Re=new bv(D,Ne),rt=new Ug(D,Ne,e,Re),ke=new Sv(D,Ne),rt.reversedDepthBuffer&&h&&ke.buffers.depth.setReversed(!0),yt=new Wg(D),Se=new lv,Je=new yv(D,Ne,ke,Se,rt,Re,yt),Dt=new Ng(v),Rt=new zg(v),R=new Zf(D),pe=new Dg(D,R),x=new Hg(D,R,yt,pe),z=new Xg(D,x,R,yt),ae=new $g(D,rt,Je),ce=new kg(Se),K=new ov(v,Dt,Rt,Ne,rt,pe,ce),ne=new Cv(v,Se),Z=new uv,Pe=new gv(Ne),Ae=new Lg(v,Dt,Rt,ke,z,p,l),we=new xv(v,z,rt),je=new Pv(D,yt,rt,ke),ge=new Ig(D,Ne,yt),Ge=new Gg(D,Ne,yt),yt.programs=K.programs,v.capabilities=rt,v.extensions=Ne,v.properties=Se,v.renderLists=Z,v.shadowMap=we,v.state=ke,v.info=yt}U();const oe=new Av(v,D);this.xr=oe,this.getContext=function(){return D},this.getContextAttributes=function(){return D.getContextAttributes()},this.forceContextLoss=function(){const M=Ne.get("WEBGL_lose_context");M&&M.loseContext()},this.forceContextRestore=function(){const M=Ne.get("WEBGL_lose_context");M&&M.restoreContext()},this.getPixelRatio=function(){return G},this.setPixelRatio=function(M){M!==void 0&&(G=M,this.setSize(B,Q,!1))},this.getSize=function(M){return M.set(B,Q)},this.setSize=function(M,k,H=!0){if(oe.isPresenting){console.warn("THREE.WebGLRenderer: Can't change size while VR device is presenting.");return}B=M,Q=k,t.width=Math.floor(M*G),t.height=Math.floor(k*G),H===!0&&(t.style.width=M+"px",t.style.height=k+"px"),this.setViewport(0,0,M,k)},this.getDrawingBufferSize=function(M){return M.set(B*G,Q*G).floor()},this.setDrawingBufferSize=function(M,k,H){B=M,Q=k,G=H,t.width=Math.floor(M*H),t.height=Math.floor(k*H),this.setViewport(0,0,M,k)},this.getCurrentViewport=function(M){return M.copy(P)},this.getViewport=function(M){return M.copy(Ee)},this.setViewport=function(M,k,H,W){M.isVector4?Ee.set(M.x,M.y,M.z,M.w):Ee.set(M,k,H,W),ke.viewport(P.copy(Ee).multiplyScalar(G).round())},this.getScissor=function(M){return M.copy(Oe)},this.setScissor=function(M,k,H,W){M.isVector4?Oe.set(M.x,M.y,M.z,M.w):Oe.set(M,k,H,W),ke.scissor($.copy(Oe).multiplyScalar(G).round())},this.getScissorTest=function(){return at},this.setScissorTest=function(M){ke.setScissorTest(at=M)},this.setOpaqueSort=function(M){ie=M},this.setTransparentSort=function(M){de=M},this.getClearColor=function(M){return M.copy(Ae.getClearColor())},this.setClearColor=function(){Ae.setClearColor(...arguments)},this.getClearAlpha=function(){return Ae.getClearAlpha()},this.setClearAlpha=function(){Ae.setClearAlpha(...arguments)},this.clear=function(M=!0,k=!0,H=!0){let W=0;if(M){let N=!1;if(I!==null){const re=I.texture.format;N=re===Eo||re===bo||re===yo}if(N){const re=I.texture.type,me=re===Mn||re===ui||re===as||re===os||re===Mo||re===So,ye=Ae.getClearColor(),ve=Ae.getClearAlpha(),Ve=ye.r,Xe=ye.g,Fe=ye.b;me?(g[0]=Ve,g[1]=Xe,g[2]=Fe,g[3]=ve,D.clearBufferuiv(D.COLOR,0,g)):(_[0]=Ve,_[1]=Xe,_[2]=Fe,_[3]=ve,D.clearBufferiv(D.COLOR,0,_))}else W|=D.COLOR_BUFFER_BIT}k&&(W|=D.DEPTH_BUFFER_BIT),H&&(W|=D.STENCIL_BUFFER_BIT,this.state.buffers.stencil.setMask(4294967295)),D.clear(W)},this.clearColor=function(){this.clear(!0,!1,!1)},this.clearDepth=function(){this.clear(!1,!0,!1)},this.clearStencil=function(){this.clear(!1,!1,!0)},this.dispose=function(){t.removeEventListener("webglcontextlost",ue,!1),t.removeEventListener("webglcontextrestored",Me,!1),t.removeEventListener("webglcontextcreationerror",se,!1),Ae.dispose(),Z.dispose(),Pe.dispose(),Se.dispose(),Dt.dispose(),Rt.dispose(),z.dispose(),pe.dispose(),je.dispose(),K.dispose(),oe.dispose(),oe.removeEventListener("sessionstart",pn),oe.removeEventListener("sessionend",Yo),Zn.stop()};function ue(M){M.preventDefault(),console.log("THREE.WebGLRenderer: Context Lost."),T=!0}function Me(){console.log("THREE.WebGLRenderer: Context Restored."),T=!1;const M=yt.autoReset,k=we.enabled,H=we.autoUpdate,W=we.needsUpdate,N=we.type;U(),yt.autoReset=M,we.enabled=k,we.autoUpdate=H,we.needsUpdate=W,we.type=N}function se(M){console.error("THREE.WebGLRenderer: A WebGL context could not be created. Reason: ",M.statusMessage)}function ee(M){const k=M.target;k.removeEventListener("dispose",ee),Te(k)}function Te(M){Ye(M),Se.remove(M)}function Ye(M){const k=Se.get(M).programs;k!==void 0&&(k.forEach(function(H){K.releaseProgram(H)}),M.isShaderMaterial&&K.releaseShaderCache(M))}this.renderBufferDirect=function(M,k,H,W,N,re){k===null&&(k=$e);const me=N.isMesh&&N.matrixWorld.determinant()<0,ye=du(M,k,H,W,N);ke.setMaterial(W,me);let ve=H.index,Ve=1;if(W.wireframe===!0){if(ve=x.getWireframeAttribute(H),ve===void 0)return;Ve=2}const Xe=H.drawRange,Fe=H.attributes.position;let nt=Xe.start*Ve,pt=(Xe.start+Xe.count)*Ve;re!==null&&(nt=Math.max(nt,re.start*Ve),pt=Math.min(pt,(re.start+re.count)*Ve)),ve!==null?(nt=Math.max(nt,0),pt=Math.min(pt,ve.count)):Fe!=null&&(nt=Math.max(nt,0),pt=Math.min(pt,Fe.count));const wt=pt-nt;if(wt<0||wt===1/0)return;pe.setup(N,W,ye,H,ve);let St,gt=ge;if(ve!==null&&(St=R.get(ve),gt=Ge,gt.setIndex(St)),N.isMesh)W.wireframe===!0?(ke.setLineWidth(W.wireframeLinewidth*st()),gt.setMode(D.LINES)):gt.setMode(D.TRIANGLES);else if(N.isLine){let ze=W.linewidth;ze===void 0&&(ze=1),ke.setLineWidth(ze*st()),N.isLineSegments?gt.setMode(D.LINES):N.isLineLoop?gt.setMode(D.LINE_LOOP):gt.setMode(D.LINE_STRIP)}else N.isPoints?gt.setMode(D.POINTS):N.isSprite&&gt.setMode(D.TRIANGLES);if(N.isBatchedMesh)if(N._multiDrawInstances!==null)Ni("THREE.WebGLRenderer: renderMultiDrawInstances has been deprecated and will be removed in r184. Append to renderMultiDraw arguments and use indirection."),gt.renderMultiDrawInstances(N._multiDrawStarts,N._multiDrawCounts,N._multiDrawCount,N._multiDrawInstances);else if(Ne.get("WEBGL_multi_draw"))gt.renderMultiDraw(N._multiDrawStarts,N._multiDrawCounts,N._multiDrawCount);else{const ze=N._multiDrawStarts,Et=N._multiDrawCounts,ot=N._multiDrawCount,Zt=ve?R.get(ve).bytesPerElement:1,gi=Se.get(W).currentProgram.getUniforms();for(let Kt=0;Kt<ot;Kt++)gi.setValue(D,"_gl_DrawID",Kt),gt.render(ze[Kt]/Zt,Et[Kt])}else if(N.isInstancedMesh)gt.renderInstances(nt,wt,N.count);else if(H.isInstancedBufferGeometry){const ze=H._maxInstanceCount!==void 0?H._maxInstanceCount:1/0,Et=Math.min(H.instanceCount,ze);gt.renderInstances(nt,wt,Et)}else gt.render(nt,wt)};function xt(M,k,H){M.transparent===!0&&M.side===qt&&M.forceSinglePass===!1?(M.side=Yt,M.needsUpdate=!0,Ss(M,k,H),M.side=qn,M.needsUpdate=!0,Ss(M,k,H),M.side=qt):Ss(M,k,H)}this.compile=function(M,k,H=null){H===null&&(H=M),f=Pe.get(H),f.init(k),b.push(f),H.traverseVisible(function(N){N.isLight&&N.layers.test(k.layers)&&(f.pushLight(N),N.castShadow&&f.pushShadow(N))}),M!==H&&M.traverseVisible(function(N){N.isLight&&N.layers.test(k.layers)&&(f.pushLight(N),N.castShadow&&f.pushShadow(N))}),f.setupLights();const W=new Set;return M.traverse(function(N){if(!(N.isMesh||N.isPoints||N.isLine||N.isSprite))return;const re=N.material;if(re)if(Array.isArray(re))for(let me=0;me<re.length;me++){const ye=re[me];xt(ye,H,N),W.add(ye)}else xt(re,H,N),W.add(re)}),f=b.pop(),W},this.compileAsync=function(M,k,H=null){const W=this.compile(M,k,H);return new Promise(N=>{function re(){if(W.forEach(function(me){Se.get(me).currentProgram.isReady()&&W.delete(me)}),W.size===0){N(M);return}setTimeout(re,10)}Ne.get("KHR_parallel_shader_compile")!==null?re():setTimeout(re,10)})};let ut=null;function yn(M){ut&&ut(M)}function pn(){Zn.stop()}function Yo(){Zn.start()}const Zn=new Wc;Zn.setAnimationLoop(yn),typeof self<"u"&&Zn.setContext(self),this.setAnimationLoop=function(M){ut=M,oe.setAnimationLoop(M),M===null?Zn.stop():Zn.start()},oe.addEventListener("sessionstart",pn),oe.addEventListener("sessionend",Yo),this.render=function(M,k){if(k!==void 0&&k.isCamera!==!0){console.error("THREE.WebGLRenderer.render: camera is not an instance of THREE.Camera.");return}if(T===!0)return;if(M.matrixWorldAutoUpdate===!0&&M.updateMatrixWorld(),k.parent===null&&k.matrixWorldAutoUpdate===!0&&k.updateMatrixWorld(),oe.enabled===!0&&oe.isPresenting===!0&&(oe.cameraAutoUpdate===!0&&oe.updateCamera(k),k=oe.getCamera()),M.isScene===!0&&M.onBeforeRender(v,M,k,I),f=Pe.get(M,b.length),f.init(k),b.push(f),le.multiplyMatrices(k.projectionMatrix,k.matrixWorldInverse),He.setFromProjectionMatrix(le,_n,k.reversedDepth),he=this.localClippingEnabled,j=ce.init(this.clippingPlanes,he),m=Z.get(M,E.length),m.init(),E.push(m),oe.enabled===!0&&oe.isPresenting===!0){const re=v.xr.getDepthSensingMesh();re!==null&&wr(re,k,-1/0,v.sortObjects)}wr(M,k,0,v.sortObjects),m.finish(),v.sortObjects===!0&&m.sort(ie,de),Mt=oe.enabled===!1||oe.isPresenting===!1||oe.hasDepthSensing()===!1,Mt&&Ae.addToRenderList(m,M),this.info.render.frame++,j===!0&&ce.beginShadows();const H=f.state.shadowsArray;we.render(H,M,k),j===!0&&ce.endShadows(),this.info.autoReset===!0&&this.info.reset();const W=m.opaque,N=m.transmissive;if(f.setupLights(),k.isArrayCamera){const re=k.cameras;if(N.length>0)for(let me=0,ye=re.length;me<ye;me++){const ve=re[me];Zo(W,N,M,ve)}Mt&&Ae.render(M);for(let me=0,ye=re.length;me<ye;me++){const ve=re[me];jo(m,M,ve,ve.viewport)}}else N.length>0&&Zo(W,N,M,k),Mt&&Ae.render(M),jo(m,M,k);I!==null&&C===0&&(Je.updateMultisampleRenderTarget(I),Je.updateRenderTargetMipmap(I)),M.isScene===!0&&M.onAfterRender(v,M,k),pe.resetDefaultState(),y=-1,S=null,b.pop(),b.length>0?(f=b[b.length-1],j===!0&&ce.setGlobalState(v.clippingPlanes,f.state.camera)):f=null,E.pop(),E.length>0?m=E[E.length-1]:m=null};function wr(M,k,H,W){if(M.visible===!1)return;if(M.layers.test(k.layers)){if(M.isGroup)H=M.renderOrder;else if(M.isLOD)M.autoUpdate===!0&&M.update(k);else if(M.isLight)f.pushLight(M),M.castShadow&&f.pushShadow(M);else if(M.isSprite){if(!M.frustumCulled||He.intersectsSprite(M)){W&&Ue.setFromMatrixPosition(M.matrixWorld).applyMatrix4(le);const me=z.update(M),ye=M.material;ye.visible&&m.push(M,me,ye,H,Ue.z,null)}}else if((M.isMesh||M.isLine||M.isPoints)&&(!M.frustumCulled||He.intersectsObject(M))){const me=z.update(M),ye=M.material;if(W&&(M.boundingSphere!==void 0?(M.boundingSphere===null&&M.computeBoundingSphere(),Ue.copy(M.boundingSphere.center)):(me.boundingSphere===null&&me.computeBoundingSphere(),Ue.copy(me.boundingSphere.center)),Ue.applyMatrix4(M.matrixWorld).applyMatrix4(le)),Array.isArray(ye)){const ve=me.groups;for(let Ve=0,Xe=ve.length;Ve<Xe;Ve++){const Fe=ve[Ve],nt=ye[Fe.materialIndex];nt&&nt.visible&&m.push(M,me,nt,H,Ue.z,Fe)}}else ye.visible&&m.push(M,me,ye,H,Ue.z,null)}}const re=M.children;for(let me=0,ye=re.length;me<ye;me++)wr(re[me],k,H,W)}function jo(M,k,H,W){const N=M.opaque,re=M.transmissive,me=M.transparent;f.setupLightsView(H),j===!0&&ce.setGlobalState(v.clippingPlanes,H),W&&ke.viewport(P.copy(W)),N.length>0&&Ms(N,k,H),re.length>0&&Ms(re,k,H),me.length>0&&Ms(me,k,H),ke.buffers.depth.setTest(!0),ke.buffers.depth.setMask(!0),ke.buffers.color.setMask(!0),ke.setPolygonOffset(!1)}function Zo(M,k,H,W){if((H.isScene===!0?H.overrideMaterial:null)!==null)return;f.state.transmissionRenderTarget[W.id]===void 0&&(f.state.transmissionRenderTarget[W.id]=new di(1,1,{generateMipmaps:!0,type:Ne.has("EXT_color_buffer_half_float")||Ne.has("EXT_color_buffer_float")?ps:Mn,minFilter:li,samples:4,stencilBuffer:r,resolveDepthBuffer:!1,resolveStencilBuffer:!1,colorSpace:lt.workingColorSpace}));const re=f.state.transmissionRenderTarget[W.id],me=W.viewport||P;re.setSize(me.z*v.transmissionResolutionScale,me.w*v.transmissionResolutionScale);const ye=v.getRenderTarget(),ve=v.getActiveCubeFace(),Ve=v.getActiveMipmapLevel();v.setRenderTarget(re),v.getClearColor(Y),X=v.getClearAlpha(),X<1&&v.setClearColor(16777215,.5),v.clear(),Mt&&Ae.render(H);const Xe=v.toneMapping;v.toneMapping=$n;const Fe=W.viewport;if(W.viewport!==void 0&&(W.viewport=void 0),f.setupLightsView(W),j===!0&&ce.setGlobalState(v.clippingPlanes,W),Ms(M,H,W),Je.updateMultisampleRenderTarget(re),Je.updateRenderTargetMipmap(re),Ne.has("WEBGL_multisampled_render_to_texture")===!1){let nt=!1;for(let pt=0,wt=k.length;pt<wt;pt++){const St=k[pt],gt=St.object,ze=St.geometry,Et=St.material,ot=St.group;if(Et.side===qt&&gt.layers.test(W.layers)){const Zt=Et.side;Et.side=Yt,Et.needsUpdate=!0,Ko(gt,H,W,ze,Et,ot),Et.side=Zt,Et.needsUpdate=!0,nt=!0}}nt===!0&&(Je.updateMultisampleRenderTarget(re),Je.updateRenderTargetMipmap(re))}v.setRenderTarget(ye,ve,Ve),v.setClearColor(Y,X),Fe!==void 0&&(W.viewport=Fe),v.toneMapping=Xe}function Ms(M,k,H){const W=k.isScene===!0?k.overrideMaterial:null;for(let N=0,re=M.length;N<re;N++){const me=M[N],ye=me.object,ve=me.geometry,Ve=me.group;let Xe=me.material;Xe.allowOverride===!0&&W!==null&&(Xe=W),ye.layers.test(H.layers)&&Ko(ye,k,H,ve,Xe,Ve)}}function Ko(M,k,H,W,N,re){M.onBeforeRender(v,k,H,W,N,re),M.modelViewMatrix.multiplyMatrices(H.matrixWorldInverse,M.matrixWorld),M.normalMatrix.getNormalMatrix(M.modelViewMatrix),N.onBeforeRender(v,k,H,W,M,re),N.transparent===!0&&N.side===qt&&N.forceSinglePass===!1?(N.side=Yt,N.needsUpdate=!0,v.renderBufferDirect(H,k,W,N,M,re),N.side=qn,N.needsUpdate=!0,v.renderBufferDirect(H,k,W,N,M,re),N.side=qt):v.renderBufferDirect(H,k,W,N,M,re),M.onAfterRender(v,k,H,W,N,re)}function Ss(M,k,H){k.isScene!==!0&&(k=$e);const W=Se.get(M),N=f.state.lights,re=f.state.shadowsArray,me=N.state.version,ye=K.getParameters(M,N.state,re,k,H),ve=K.getProgramCacheKey(ye);let Ve=W.programs;W.environment=M.isMeshStandardMaterial?k.environment:null,W.fog=k.fog,W.envMap=(M.isMeshStandardMaterial?Rt:Dt).get(M.envMap||W.environment),W.envMapRotation=W.environment!==null&&M.envMap===null?k.environmentRotation:M.envMapRotation,Ve===void 0&&(M.addEventListener("dispose",ee),Ve=new Map,W.programs=Ve);let Xe=Ve.get(ve);if(Xe!==void 0){if(W.currentProgram===Xe&&W.lightsStateVersion===me)return Qo(M,ye),Xe}else ye.uniforms=K.getUniforms(M),M.onBeforeCompile(ye,v),Xe=K.acquireProgram(ye,ve),Ve.set(ve,Xe),W.uniforms=ye.uniforms;const Fe=W.uniforms;return(!M.isShaderMaterial&&!M.isRawShaderMaterial||M.clipping===!0)&&(Fe.clippingPlanes=ce.uniform),Qo(M,ye),W.needsLights=fu(M),W.lightsStateVersion=me,W.needsLights&&(Fe.ambientLightColor.value=N.state.ambient,Fe.lightProbe.value=N.state.probe,Fe.directionalLights.value=N.state.directional,Fe.directionalLightShadows.value=N.state.directionalShadow,Fe.spotLights.value=N.state.spot,Fe.spotLightShadows.value=N.state.spotShadow,Fe.rectAreaLights.value=N.state.rectArea,Fe.ltc_1.value=N.state.rectAreaLTC1,Fe.ltc_2.value=N.state.rectAreaLTC2,Fe.pointLights.value=N.state.point,Fe.pointLightShadows.value=N.state.pointShadow,Fe.hemisphereLights.value=N.state.hemi,Fe.directionalShadowMap.value=N.state.directionalShadowMap,Fe.directionalShadowMatrix.value=N.state.directionalShadowMatrix,Fe.spotShadowMap.value=N.state.spotShadowMap,Fe.spotLightMatrix.value=N.state.spotLightMatrix,Fe.spotLightMap.value=N.state.spotLightMap,Fe.pointShadowMap.value=N.state.pointShadowMap,Fe.pointShadowMatrix.value=N.state.pointShadowMatrix),W.currentProgram=Xe,W.uniformsList=null,Xe}function Jo(M){if(M.uniformsList===null){const k=M.currentProgram.getUniforms();M.uniformsList=tr.seqWithValue(k.seq,M.uniforms)}return M.uniformsList}function Qo(M,k){const H=Se.get(M);H.outputColorSpace=k.outputColorSpace,H.batching=k.batching,H.batchingColor=k.batchingColor,H.instancing=k.instancing,H.instancingColor=k.instancingColor,H.instancingMorph=k.instancingMorph,H.skinning=k.skinning,H.morphTargets=k.morphTargets,H.morphNormals=k.morphNormals,H.morphColors=k.morphColors,H.morphTargetsCount=k.morphTargetsCount,H.numClippingPlanes=k.numClippingPlanes,H.numIntersection=k.numClipIntersection,H.vertexAlphas=k.vertexAlphas,H.vertexTangents=k.vertexTangents,H.toneMapping=k.toneMapping}function du(M,k,H,W,N){k.isScene!==!0&&(k=$e),Je.resetTextureUnits();const re=k.fog,me=W.isMeshStandardMaterial?k.environment:null,ye=I===null?v.outputColorSpace:I.isXRRenderTarget===!0?I.texture.colorSpace:Hi,ve=(W.isMeshStandardMaterial?Rt:Dt).get(W.envMap||me),Ve=W.vertexColors===!0&&!!H.attributes.color&&H.attributes.color.itemSize===4,Xe=!!H.attributes.tangent&&(!!W.normalMap||W.anisotropy>0),Fe=!!H.morphAttributes.position,nt=!!H.morphAttributes.normal,pt=!!H.morphAttributes.color;let wt=$n;W.toneMapped&&(I===null||I.isXRRenderTarget===!0)&&(wt=v.toneMapping);const St=H.morphAttributes.position||H.morphAttributes.normal||H.morphAttributes.color,gt=St!==void 0?St.length:0,ze=Se.get(W),Et=f.state.lights;if(j===!0&&(he===!0||M!==S)){const Vt=M===S&&W.id===y;ce.setState(W,M,Vt)}let ot=!1;W.version===ze.__version?(ze.needsLights&&ze.lightsStateVersion!==Et.state.version||ze.outputColorSpace!==ye||N.isBatchedMesh&&ze.batching===!1||!N.isBatchedMesh&&ze.batching===!0||N.isBatchedMesh&&ze.batchingColor===!0&&N.colorTexture===null||N.isBatchedMesh&&ze.batchingColor===!1&&N.colorTexture!==null||N.isInstancedMesh&&ze.instancing===!1||!N.isInstancedMesh&&ze.instancing===!0||N.isSkinnedMesh&&ze.skinning===!1||!N.isSkinnedMesh&&ze.skinning===!0||N.isInstancedMesh&&ze.instancingColor===!0&&N.instanceColor===null||N.isInstancedMesh&&ze.instancingColor===!1&&N.instanceColor!==null||N.isInstancedMesh&&ze.instancingMorph===!0&&N.morphTexture===null||N.isInstancedMesh&&ze.instancingMorph===!1&&N.morphTexture!==null||ze.envMap!==ve||W.fog===!0&&ze.fog!==re||ze.numClippingPlanes!==void 0&&(ze.numClippingPlanes!==ce.numPlanes||ze.numIntersection!==ce.numIntersection)||ze.vertexAlphas!==Ve||ze.vertexTangents!==Xe||ze.morphTargets!==Fe||ze.morphNormals!==nt||ze.morphColors!==pt||ze.toneMapping!==wt||ze.morphTargetsCount!==gt)&&(ot=!0):(ot=!0,ze.__version=W.version);let Zt=ze.currentProgram;ot===!0&&(Zt=Ss(W,k,N));let gi=!1,Kt=!1,qi=!1;const Tt=Zt.getUniforms(),tn=ze.uniforms;if(ke.useProgram(Zt.program)&&(gi=!0,Kt=!0,qi=!0),W.id!==y&&(y=W.id,Kt=!0),gi||S!==M){ke.buffers.depth.getReversed()&&M.reversedDepth!==!0&&(M._reversedDepth=!0,M.updateProjectionMatrix()),Tt.setValue(D,"projectionMatrix",M.projectionMatrix),Tt.setValue(D,"viewMatrix",M.matrixWorldInverse);const Xt=Tt.map.cameraPosition;Xt!==void 0&&Xt.setValue(D,Be.setFromMatrixPosition(M.matrixWorld)),rt.logarithmicDepthBuffer&&Tt.setValue(D,"logDepthBufFC",2/(Math.log(M.far+1)/Math.LN2)),(W.isMeshPhongMaterial||W.isMeshToonMaterial||W.isMeshLambertMaterial||W.isMeshBasicMaterial||W.isMeshStandardMaterial||W.isShaderMaterial)&&Tt.setValue(D,"isOrthographic",M.isOrthographicCamera===!0),S!==M&&(S=M,Kt=!0,qi=!0)}if(N.isSkinnedMesh){Tt.setOptional(D,N,"bindMatrix"),Tt.setOptional(D,N,"bindMatrixInverse");const Vt=N.skeleton;Vt&&(Vt.boneTexture===null&&Vt.computeBoneTexture(),Tt.setValue(D,"boneTexture",Vt.boneTexture,Je))}N.isBatchedMesh&&(Tt.setOptional(D,N,"batchingTexture"),Tt.setValue(D,"batchingTexture",N._matricesTexture,Je),Tt.setOptional(D,N,"batchingIdTexture"),Tt.setValue(D,"batchingIdTexture",N._indirectTexture,Je),Tt.setOptional(D,N,"batchingColorTexture"),N._colorsTexture!==null&&Tt.setValue(D,"batchingColorTexture",N._colorsTexture,Je));const nn=H.morphAttributes;if((nn.position!==void 0||nn.normal!==void 0||nn.color!==void 0)&&ae.update(N,H,Zt),(Kt||ze.receiveShadow!==N.receiveShadow)&&(ze.receiveShadow=N.receiveShadow,Tt.setValue(D,"receiveShadow",N.receiveShadow)),W.isMeshGouraudMaterial&&W.envMap!==null&&(tn.envMap.value=ve,tn.flipEnvMap.value=ve.isCubeTexture&&ve.isRenderTargetTexture===!1?-1:1),W.isMeshStandardMaterial&&W.envMap===null&&k.environment!==null&&(tn.envMapIntensity.value=k.environmentIntensity),Kt&&(Tt.setValue(D,"toneMappingExposure",v.toneMappingExposure),ze.needsLights&&hu(tn,qi),re&&W.fog===!0&&ne.refreshFogUniforms(tn,re),ne.refreshMaterialUniforms(tn,W,G,Q,f.state.transmissionRenderTarget[M.id]),tr.upload(D,Jo(ze),tn,Je)),W.isShaderMaterial&&W.uniformsNeedUpdate===!0&&(tr.upload(D,Jo(ze),tn,Je),W.uniformsNeedUpdate=!1),W.isSpriteMaterial&&Tt.setValue(D,"center",N.center),Tt.setValue(D,"modelViewMatrix",N.modelViewMatrix),Tt.setValue(D,"normalMatrix",N.normalMatrix),Tt.setValue(D,"modelMatrix",N.matrixWorld),W.isShaderMaterial||W.isRawShaderMaterial){const Vt=W.uniformsGroups;for(let Xt=0,Ar=Vt.length;Xt<Ar;Xt++){const Kn=Vt[Xt];je.update(Kn,Zt),je.bind(Kn,Zt)}}return Zt}function hu(M,k){M.ambientLightColor.needsUpdate=k,M.lightProbe.needsUpdate=k,M.directionalLights.needsUpdate=k,M.directionalLightShadows.needsUpdate=k,M.pointLights.needsUpdate=k,M.pointLightShadows.needsUpdate=k,M.spotLights.needsUpdate=k,M.spotLightShadows.needsUpdate=k,M.rectAreaLights.needsUpdate=k,M.hemisphereLights.needsUpdate=k}function fu(M){return M.isMeshLambertMaterial||M.isMeshToonMaterial||M.isMeshPhongMaterial||M.isMeshStandardMaterial||M.isShadowMaterial||M.isShaderMaterial&&M.lights===!0}this.getActiveCubeFace=function(){return A},this.getActiveMipmapLevel=function(){return C},this.getRenderTarget=function(){return I},this.setRenderTargetTextures=function(M,k,H){const W=Se.get(M);W.__autoAllocateDepthBuffer=M.resolveDepthBuffer===!1,W.__autoAllocateDepthBuffer===!1&&(W.__useRenderToTexture=!1),Se.get(M.texture).__webglTexture=k,Se.get(M.depthTexture).__webglTexture=W.__autoAllocateDepthBuffer?void 0:H,W.__hasExternalTextures=!0},this.setRenderTargetFramebuffer=function(M,k){const H=Se.get(M);H.__webglFramebuffer=k,H.__useDefaultFramebuffer=k===void 0};const pu=D.createFramebuffer();this.setRenderTarget=function(M,k=0,H=0){I=M,A=k,C=H;let W=!0,N=null,re=!1,me=!1;if(M){const ve=Se.get(M);if(ve.__useDefaultFramebuffer!==void 0)ke.bindFramebuffer(D.FRAMEBUFFER,null),W=!1;else if(ve.__webglFramebuffer===void 0)Je.setupRenderTarget(M);else if(ve.__hasExternalTextures)Je.rebindTextures(M,Se.get(M.texture).__webglTexture,Se.get(M.depthTexture).__webglTexture);else if(M.depthBuffer){const Fe=M.depthTexture;if(ve.__boundDepthTexture!==Fe){if(Fe!==null&&Se.has(Fe)&&(M.width!==Fe.image.width||M.height!==Fe.image.height))throw new Error("WebGLRenderTarget: Attached DepthTexture is initialized to the incorrect size.");Je.setupDepthRenderbuffer(M)}}const Ve=M.texture;(Ve.isData3DTexture||Ve.isDataArrayTexture||Ve.isCompressedArrayTexture)&&(me=!0);const Xe=Se.get(M).__webglFramebuffer;M.isWebGLCubeRenderTarget?(Array.isArray(Xe[k])?N=Xe[k][H]:N=Xe[k],re=!0):M.samples>0&&Je.useMultisampledRTT(M)===!1?N=Se.get(M).__webglMultisampledFramebuffer:Array.isArray(Xe)?N=Xe[H]:N=Xe,P.copy(M.viewport),$.copy(M.scissor),q=M.scissorTest}else P.copy(Ee).multiplyScalar(G).floor(),$.copy(Oe).multiplyScalar(G).floor(),q=at;if(H!==0&&(N=pu),ke.bindFramebuffer(D.FRAMEBUFFER,N)&&W&&ke.drawBuffers(M,N),ke.viewport(P),ke.scissor($),ke.setScissorTest(q),re){const ve=Se.get(M.texture);D.framebufferTexture2D(D.FRAMEBUFFER,D.COLOR_ATTACHMENT0,D.TEXTURE_CUBE_MAP_POSITIVE_X+k,ve.__webglTexture,H)}else if(me){const ve=k;for(let Ve=0;Ve<M.textures.length;Ve++){const Xe=Se.get(M.textures[Ve]);D.framebufferTextureLayer(D.FRAMEBUFFER,D.COLOR_ATTACHMENT0+Ve,Xe.__webglTexture,H,ve)}}else if(M!==null&&H!==0){const ve=Se.get(M.texture);D.framebufferTexture2D(D.FRAMEBUFFER,D.COLOR_ATTACHMENT0,D.TEXTURE_2D,ve.__webglTexture,H)}y=-1},this.readRenderTargetPixels=function(M,k,H,W,N,re,me,ye=0){if(!(M&&M.isWebGLRenderTarget)){console.error("THREE.WebGLRenderer.readRenderTargetPixels: renderTarget is not THREE.WebGLRenderTarget.");return}let ve=Se.get(M).__webglFramebuffer;if(M.isWebGLCubeRenderTarget&&me!==void 0&&(ve=ve[me]),ve){ke.bindFramebuffer(D.FRAMEBUFFER,ve);try{const Ve=M.textures[ye],Xe=Ve.format,Fe=Ve.type;if(!rt.textureFormatReadable(Xe)){console.error("THREE.WebGLRenderer.readRenderTargetPixels: renderTarget is not in RGBA or implementation defined format.");return}if(!rt.textureTypeReadable(Fe)){console.error("THREE.WebGLRenderer.readRenderTargetPixels: renderTarget is not in UnsignedByteType or implementation defined type.");return}k>=0&&k<=M.width-W&&H>=0&&H<=M.height-N&&(M.textures.length>1&&D.readBuffer(D.COLOR_ATTACHMENT0+ye),D.readPixels(k,H,W,N,Re.convert(Xe),Re.convert(Fe),re))}finally{const Ve=I!==null?Se.get(I).__webglFramebuffer:null;ke.bindFramebuffer(D.FRAMEBUFFER,Ve)}}},this.readRenderTargetPixelsAsync=async function(M,k,H,W,N,re,me,ye=0){if(!(M&&M.isWebGLRenderTarget))throw new Error("THREE.WebGLRenderer.readRenderTargetPixels: renderTarget is not THREE.WebGLRenderTarget.");let ve=Se.get(M).__webglFramebuffer;if(M.isWebGLCubeRenderTarget&&me!==void 0&&(ve=ve[me]),ve)if(k>=0&&k<=M.width-W&&H>=0&&H<=M.height-N){ke.bindFramebuffer(D.FRAMEBUFFER,ve);const Ve=M.textures[ye],Xe=Ve.format,Fe=Ve.type;if(!rt.textureFormatReadable(Xe))throw new Error("THREE.WebGLRenderer.readRenderTargetPixelsAsync: renderTarget is not in RGBA or implementation defined format.");if(!rt.textureTypeReadable(Fe))throw new Error("THREE.WebGLRenderer.readRenderTargetPixelsAsync: renderTarget is not in UnsignedByteType or implementation defined type.");const nt=D.createBuffer();D.bindBuffer(D.PIXEL_PACK_BUFFER,nt),D.bufferData(D.PIXEL_PACK_BUFFER,re.byteLength,D.STREAM_READ),M.textures.length>1&&D.readBuffer(D.COLOR_ATTACHMENT0+ye),D.readPixels(k,H,W,N,Re.convert(Xe),Re.convert(Fe),0);const pt=I!==null?Se.get(I).__webglFramebuffer:null;ke.bindFramebuffer(D.FRAMEBUFFER,pt);const wt=D.fenceSync(D.SYNC_GPU_COMMANDS_COMPLETE,0);return D.flush(),await of(D,wt,4),D.bindBuffer(D.PIXEL_PACK_BUFFER,nt),D.getBufferSubData(D.PIXEL_PACK_BUFFER,0,re),D.deleteBuffer(nt),D.deleteSync(wt),re}else throw new Error("THREE.WebGLRenderer.readRenderTargetPixelsAsync: requested read bounds are out of range.")},this.copyFramebufferToTexture=function(M,k=null,H=0){const W=Math.pow(2,-H),N=Math.floor(M.image.width*W),re=Math.floor(M.image.height*W),me=k!==null?k.x:0,ye=k!==null?k.y:0;Je.setTexture2D(M,0),D.copyTexSubImage2D(D.TEXTURE_2D,H,0,0,me,ye,N,re),ke.unbindTexture()};const mu=D.createFramebuffer(),gu=D.createFramebuffer();this.copyTextureToTexture=function(M,k,H=null,W=null,N=0,re=null){re===null&&(N!==0?(Ni("WebGLRenderer: copyTextureToTexture function signature has changed to support src and dst mipmap levels."),re=N,N=0):re=0);let me,ye,ve,Ve,Xe,Fe,nt,pt,wt;const St=M.isCompressedTexture?M.mipmaps[re]:M.image;if(H!==null)me=H.max.x-H.min.x,ye=H.max.y-H.min.y,ve=H.isBox3?H.max.z-H.min.z:1,Ve=H.min.x,Xe=H.min.y,Fe=H.isBox3?H.min.z:0;else{const nn=Math.pow(2,-N);me=Math.floor(St.width*nn),ye=Math.floor(St.height*nn),M.isDataArrayTexture?ve=St.depth:M.isData3DTexture?ve=Math.floor(St.depth*nn):ve=1,Ve=0,Xe=0,Fe=0}W!==null?(nt=W.x,pt=W.y,wt=W.z):(nt=0,pt=0,wt=0);const gt=Re.convert(k.format),ze=Re.convert(k.type);let Et;k.isData3DTexture?(Je.setTexture3D(k,0),Et=D.TEXTURE_3D):k.isDataArrayTexture||k.isCompressedArrayTexture?(Je.setTexture2DArray(k,0),Et=D.TEXTURE_2D_ARRAY):(Je.setTexture2D(k,0),Et=D.TEXTURE_2D),D.pixelStorei(D.UNPACK_FLIP_Y_WEBGL,k.flipY),D.pixelStorei(D.UNPACK_PREMULTIPLY_ALPHA_WEBGL,k.premultiplyAlpha),D.pixelStorei(D.UNPACK_ALIGNMENT,k.unpackAlignment);const ot=D.getParameter(D.UNPACK_ROW_LENGTH),Zt=D.getParameter(D.UNPACK_IMAGE_HEIGHT),gi=D.getParameter(D.UNPACK_SKIP_PIXELS),Kt=D.getParameter(D.UNPACK_SKIP_ROWS),qi=D.getParameter(D.UNPACK_SKIP_IMAGES);D.pixelStorei(D.UNPACK_ROW_LENGTH,St.width),D.pixelStorei(D.UNPACK_IMAGE_HEIGHT,St.height),D.pixelStorei(D.UNPACK_SKIP_PIXELS,Ve),D.pixelStorei(D.UNPACK_SKIP_ROWS,Xe),D.pixelStorei(D.UNPACK_SKIP_IMAGES,Fe);const Tt=M.isDataArrayTexture||M.isData3DTexture,tn=k.isDataArrayTexture||k.isData3DTexture;if(M.isDepthTexture){const nn=Se.get(M),Vt=Se.get(k),Xt=Se.get(nn.__renderTarget),Ar=Se.get(Vt.__renderTarget);ke.bindFramebuffer(D.READ_FRAMEBUFFER,Xt.__webglFramebuffer),ke.bindFramebuffer(D.DRAW_FRAMEBUFFER,Ar.__webglFramebuffer);for(let Kn=0;Kn<ve;Kn++)Tt&&(D.framebufferTextureLayer(D.READ_FRAMEBUFFER,D.COLOR_ATTACHMENT0,Se.get(M).__webglTexture,N,Fe+Kn),D.framebufferTextureLayer(D.DRAW_FRAMEBUFFER,D.COLOR_ATTACHMENT0,Se.get(k).__webglTexture,re,wt+Kn)),D.blitFramebuffer(Ve,Xe,me,ye,nt,pt,me,ye,D.DEPTH_BUFFER_BIT,D.NEAREST);ke.bindFramebuffer(D.READ_FRAMEBUFFER,null),ke.bindFramebuffer(D.DRAW_FRAMEBUFFER,null)}else if(N!==0||M.isRenderTargetTexture||Se.has(M)){const nn=Se.get(M),Vt=Se.get(k);ke.bindFramebuffer(D.READ_FRAMEBUFFER,mu),ke.bindFramebuffer(D.DRAW_FRAMEBUFFER,gu);for(let Xt=0;Xt<ve;Xt++)Tt?D.framebufferTextureLayer(D.READ_FRAMEBUFFER,D.COLOR_ATTACHMENT0,nn.__webglTexture,N,Fe+Xt):D.framebufferTexture2D(D.READ_FRAMEBUFFER,D.COLOR_ATTACHMENT0,D.TEXTURE_2D,nn.__webglTexture,N),tn?D.framebufferTextureLayer(D.DRAW_FRAMEBUFFER,D.COLOR_ATTACHMENT0,Vt.__webglTexture,re,wt+Xt):D.framebufferTexture2D(D.DRAW_FRAMEBUFFER,D.COLOR_ATTACHMENT0,D.TEXTURE_2D,Vt.__webglTexture,re),N!==0?D.blitFramebuffer(Ve,Xe,me,ye,nt,pt,me,ye,D.COLOR_BUFFER_BIT,D.NEAREST):tn?D.copyTexSubImage3D(Et,re,nt,pt,wt+Xt,Ve,Xe,me,ye):D.copyTexSubImage2D(Et,re,nt,pt,Ve,Xe,me,ye);ke.bindFramebuffer(D.READ_FRAMEBUFFER,null),ke.bindFramebuffer(D.DRAW_FRAMEBUFFER,null)}else tn?M.isDataTexture||M.isData3DTexture?D.texSubImage3D(Et,re,nt,pt,wt,me,ye,ve,gt,ze,St.data):k.isCompressedArrayTexture?D.compressedTexSubImage3D(Et,re,nt,pt,wt,me,ye,ve,gt,St.data):D.texSubImage3D(Et,re,nt,pt,wt,me,ye,ve,gt,ze,St):M.isDataTexture?D.texSubImage2D(D.TEXTURE_2D,re,nt,pt,me,ye,gt,ze,St.data):M.isCompressedTexture?D.compressedTexSubImage2D(D.TEXTURE_2D,re,nt,pt,St.width,St.height,gt,St.data):D.texSubImage2D(D.TEXTURE_2D,re,nt,pt,me,ye,gt,ze,St);D.pixelStorei(D.UNPACK_ROW_LENGTH,ot),D.pixelStorei(D.UNPACK_IMAGE_HEIGHT,Zt),D.pixelStorei(D.UNPACK_SKIP_PIXELS,gi),D.pixelStorei(D.UNPACK_SKIP_ROWS,Kt),D.pixelStorei(D.UNPACK_SKIP_IMAGES,qi),re===0&&k.generateMipmaps&&D.generateMipmap(Et),ke.unbindTexture()},this.copyTextureToTexture3D=function(M,k,H=null,W=null,N=0){return Ni('WebGLRenderer: copyTextureToTexture3D function has been deprecated. Use "copyTextureToTexture" instead.'),this.copyTextureToTexture(M,k,H,W,N)},this.initRenderTarget=function(M){Se.get(M).__webglFramebuffer===void 0&&Je.setupRenderTarget(M)},this.initTexture=function(M){M.isCubeTexture?Je.setTextureCube(M,0):M.isData3DTexture?Je.setTexture3D(M,0):M.isDataArrayTexture||M.isCompressedArrayTexture?Je.setTexture2DArray(M,0):Je.setTexture2D(M,0),ke.unbindTexture()},this.resetState=function(){A=0,C=0,I=null,ke.reset(),pe.reset()},typeof __THREE_DEVTOOLS__<"u"&&__THREE_DEVTOOLS__.dispatchEvent(new CustomEvent("observe",{detail:this}))}get coordinateSystem(){return _n}get outputColorSpace(){return this._outputColorSpace}set outputColorSpace(e){this._outputColorSpace=e;const t=this.getContext();t.drawingBufferColorSpace=lt._getDrawingBufferColorSpace(e),t.unpackColorSpace=lt._getUnpackColorSpace()}}const Lv={__name:"Hero3DEffect",props:{mouseX:{type:Number,default:0},mouseY:{type:Number,default:0}},setup(i){const e=i,t=We(null),n=We(null);let s=null,r,a,o,l,c={x:0,y:0};const u=qe(()=>typeof document<"u"?document.documentElement.classList.contains("dark"):!1),d=qe(()=>u.value?{nodeColor:6514417,connectionColor:9133302,ringColors:[9133302,440020,1096065],particleColors:[new Le(6514417),new Le(9133302),new Le(440020),new Le(1096065)]}:{nodeColor:4405450,connectionColor:6514417,ringColors:[8141549,561586,366185],particleColors:[new Le(4405450),new Le(8141549),new Le(561586),new Le(366185)]});zt(()=>{h(),f(),window.addEventListener("resize",E);const v=new MutationObserver(()=>{b()});typeof document<"u"&&v.observe(document.documentElement,{attributes:!0,attributeFilter:["class"]}),ha(()=>{v.disconnect()})}),ha(()=>{s&&cancelAnimationFrame(s),window.removeEventListener("resize",E)});function h(){r=new xr,a=new Wt(75,t.value.clientWidth/t.value.clientHeight,.1,1e3),a.position.set(0,0,50),o=new yr({canvas:n.value,alpha:!0,antialias:!0}),o.setSize(t.value.clientWidth,t.value.clientHeight),o.setClearColor(0,0),l=new ts,p(),g(),_(),m(),r.add(l)}function p(){const v=new Lo(.3,8,6),T=new hs({color:d.value.nodeColor,transparent:!0,opacity:.7});[{x:0,y:0,z:0,scale:1.5},{x:15,y:8,z:-5,scale:1},{x:-12,y:-6,z:8,scale:1},{x:8,y:-10,z:12,scale:1},{x:-18,y:5,z:-8,scale:1},{x:10,y:15,z:3,scale:1}].forEach(C=>{const I=new kt(v,T.clone());I.position.set(C.x,C.y,C.z),I.scale.setScalar(C.scale),I.userData={originalScale:C.scale,type:"node"},l.add(I)})}function g(){const v=new Bc({color:d.value.connectionColor,transparent:!0,opacity:.25}),T=[[0,1],[0,2],[0,3],[0,4],[0,5],[1,3],[2,4],[3,5],[1,5]],A=[new O(0,0,0),new O(15,8,-5),new O(-12,-6,8),new O(8,-10,12),new O(-18,5,-8),new O(10,15,3)];T.forEach(([C,I])=>{const y=new jt().setFromPoints([A[C],A[I]]),S=new kf(y,v);S.userData={type:"connection"},l.add(S)})}function _(){[{radius:25,segments:64,opacity:.12},{radius:35,segments:80,opacity:.08},{radius:45,segments:96,opacity:.06}].forEach((T,A)=>{const C=new Mr(T.radius-.5,T.radius+.5,T.segments),I=new hs({color:d.value.ringColors[A],transparent:!0,opacity:T.opacity,side:qt}),y=new kt(C,I);y.rotation.x=Math.PI/2+A*.3,y.rotation.z=A*.5,y.userData={type:"ring",index:A,ringIndex:A},l.add(y)})}function m(){const v=new jt,T=200,A=new Float32Array(T*3),C=new Float32Array(T*3),I=new Float32Array(T),y=d.value.particleColors;for(let $=0;$<T;$++){const q=$/T*Math.PI*2,Y=20+Math.random()*30,X=(Math.random()-.5)*10;A[$*3]=Math.cos(q)*Y+(Math.random()-.5)*5,A[$*3+1]=X,A[$*3+2]=Math.sin(q)*Y+(Math.random()-.5)*5,I[$]=Math.random()*.8+.2;const B=y[Math.floor(Math.random()*y.length)];C[$*3]=B.r,C[$*3+1]=B.g,C[$*3+2]=B.b}v.setAttribute("position",new en(A,3)),v.setAttribute("color",new en(C,3)),v.setAttribute("size",new en(I,1));const S=new zc({size:1.2,sizeAttenuation:!0,vertexColors:!0,transparent:!0,opacity:.5,blending:sr}),P=new Nf(v,S);P.userData={type:"dataFlow"},l.add(P)}function f(){if(s=requestAnimationFrame(f),a&&(a.position.x=c.x+e.mouseX*.5,a.position.y=c.y-e.mouseY*.5,a.lookAt(0,0,0)),l){const v=Date.now()*.001;l.rotation.y+=.002,l.children.forEach((T,A)=>{if(T.userData.type==="node"){const C=Math.sin(v*2+A)*.2+1;T.scale.setScalar(T.userData.originalScale*C),T.material.opacity=.6+Math.sin(v*3+A)*.2}else if(T.userData.type==="ring"){const C=T.userData.index;T.rotation.z+=.001+C*5e-4,T.material.opacity=T.material.opacity*.99+(.05+Math.sin(v+C)*.03)*.01}else if(T.userData.type==="connection")T.material.opacity=.2+Math.sin(v*1.5+A*.5)*.1;else if(T.userData.type==="dataFlow"&&T.geometry&&T.geometry.attributes){const C=T.geometry.attributes.position.array,I=T.geometry.attributes.size.array;for(let y=0;y<C.length;y+=3){const S=y/3,P=v*.5+S*.1,$=20+Math.sin(v+S)*10;C[y]=Math.cos(P)*$+Math.sin(v*2+S)*2,C[y+1]=Math.sin(v*.8+S)*5,C[y+2]=Math.sin(P)*$+Math.cos(v*1.5+S)*2,I[S]=.5+Math.sin(v*4+S)*.3}T.geometry.attributes.position.needsUpdate=!0,T.geometry.attributes.size.needsUpdate=!0}})}o&&r&&a&&o.render(r,a)}function E(){a&&o&&t.value&&(a.aspect=t.value.clientWidth/t.value.clientHeight,a.updateProjectionMatrix(),o.setSize(t.value.clientWidth,t.value.clientHeight))}function b(){r&&r.traverse(v=>{if(v.isMesh&&v.material&&v.geometry.type==="SphereGeometry"&&v.material.color.setHex(d.value.nodeColor),v.isLine&&v.material&&v.material.color.setHex(d.value.connectionColor),v.isMesh&&v.material&&v.geometry.type==="RingGeometry"){const T=v.userData.ringIndex||0;v.material.color.setHex(d.value.ringColors[T])}if(v.isPoints&&v.geometry.attributes.color){const T=v.geometry.attributes.color.array,A=d.value.particleColors;for(let C=0;C<T.length;C+=3){const I=Math.floor(C/3%A.length),y=A[I];T[C]=y.r,T[C+1]=y.g,T[C+2]=y.b}v.geometry.attributes.color.needsUpdate=!0}})}return(v,T)=>(L(),F("div",{class:"hero-3d-effect",ref_key:"container",ref:t},[w("canvas",{ref_key:"canvas",ref:n,class:"hero-3d-canvas"},null,512)],512))}},Zc=be(Lv,[["__scopeId","data-v-ee6e4f31"]]),Dv={class:"custom-hero-container"},Iv={class:"hero-3d-effect-wrapper"},Uv={__name:"CustomHomeHero",setup(i){const{frontmatter:e}=Qe(),t=We(null),n=We(0),s=We(0),r=We(0),a=l=>{n.value=(l.clientX/window.innerWidth-.5)*2,s.value=(l.clientY/window.innerHeight-.5)*2},o=()=>{r.value=window.scrollY};return zt(()=>{window.addEventListener("mousemove",a),window.addEventListener("scroll",o)}),jn(()=>{window.removeEventListener("mousemove",a),window.removeEventListener("scroll",o)}),(l,c)=>(L(),F("div",Dv,[w("div",Iv,[Ie(Zc,{"mouse-x":n.value,"mouse-y":s.value},null,8,["mouse-x","mouse-y"])]),w("div",{ref_key:"heroRef",ref:t,class:"hero-content-wrapper"},[V(e).hero?(L(),De(nh,{key:0,class:"VPHomeHero glassmorphism",name:V(e).hero.name,text:V(e).hero.text,tagline:V(e).hero.tagline,image:V(e).hero.image,actions:V(e).hero.actions},{"home-hero-info-before":te(()=>[J(l.$slots,"home-hero-info-before",{},void 0,!0)]),"home-hero-info":te(()=>[J(l.$slots,"home-hero-info",{},void 0,!0)]),"home-hero-info-after":te(()=>[J(l.$slots,"home-hero-info-after",{},void 0,!0)]),"home-hero-actions":te(()=>c[0]||(c[0]=[w("div",{class:"VPHeroActions"},[w("div",{class:"action"},[w("a",{class:"VPButton medium brand",href:"/manual/quick-start"},"Get Started")]),w("div",{class:"action"},[w("a",{class:"VPButton medium alt",href:"/manual/introduction"},"Learn More")]),w("div",{class:"action"},[w("a",{class:"VPButton medium alt github-button",href:"https://github.com/M-CreativeLab/jsar-runtime",target:"_blank",rel:"noreferrer"},[w("svg",{class:"github-icon",viewBox:"0 0 24 24",width:"16",height:"16"},[w("path",{fill:"currentColor",d:"M12 0c-6.626 0-12 5.373-12 12 0 5.302 3.438 9.8 8.207 11.387.599.111.793-.261.793-.577v-2.234c-3.338.726-4.033-1.416-4.033-1.416-.546-1.387-1.333-1.756-1.333-1.756-1.089-.745.083-.729.083-.729 1.205.084 1.839 1.237 1.839 1.237 1.07 1.834 2.807 1.304 3.492.997.107-.775.418-1.305.762-1.604-2.665-.305-5.467-1.334-5.467-5.931 0-1.311.469-2.381 1.236-3.221-.124-.303-.535-1.524.117-3.176 0 0 1.008-.322 3.301 1.23.957-.266 1.983-.399 3.003-.404 1.02.005 2.047.138 3.006.404 2.291-1.552 3.297-1.23 3.297-1.23.653 1.653.242 2.874.118 3.176.77.84 1.235 1.911 1.235 3.221 0 4.609-2.807 5.624-5.479 5.921.43.372.823 1.102.823 2.222v3.293c0 .319.192.694.801.576 4.765-1.589 8.199-6.086 8.199-11.386 0-6.627-5.373-12-12-12z"})]),Xn(" View on GitHub ")])])],-1)])),"home-hero-actions-after":te(()=>[J(l.$slots,"home-hero-actions-after",{},void 0,!0)]),"home-hero-image":te(()=>[J(l.$slots,"home-hero-image",{},void 0,!0)]),_:3},8,["name","text","tagline","image","actions"])):_e("",!0)],512)]))}},kv=be(Uv,[["__scopeId","data-v-6ac95011"]]),Nv={class:"box"},Fv={key:0,class:"icon"},Ov=["innerHTML"],Bv=["innerHTML"],zv=["innerHTML"],Vv={key:4,class:"link-text"},Hv={class:"link-text-value"},Gv=Ce({__name:"VPFeature",props:{icon:{},title:{},details:{},link:{},linkText:{},rel:{},target:{}},setup(i){return(e,t)=>(L(),De(xn,{class:"VPFeature",href:e.link,rel:e.rel,target:e.target,"no-icon":!0,tag:e.link?"a":"div"},{default:te(()=>[w("article",Nv,[typeof e.icon=="object"&&e.icon.wrap?(L(),F("div",Fv,[Ie(ir,{image:e.icon,alt:e.icon.alt,height:e.icon.height||48,width:e.icon.width||48},null,8,["image","alt","height","width"])])):typeof e.icon=="object"?(L(),De(ir,{key:1,image:e.icon,alt:e.icon.alt,height:e.icon.height||48,width:e.icon.width||48},null,8,["image","alt","height","width"])):e.icon?(L(),F("div",{key:2,class:"icon",innerHTML:e.icon},null,8,Ov)):_e("",!0),w("h2",{class:"title",innerHTML:e.title},null,8,Bv),e.details?(L(),F("p",{key:3,class:"details",innerHTML:e.details},null,8,zv)):_e("",!0),e.linkText?(L(),F("div",Vv,[w("p",Hv,[Xn(xe(e.linkText)+" ",1),t[0]||(t[0]=w("span",{class:"vpi-arrow-right link-text-icon"},null,-1))])])):_e("",!0)])]),_:1},8,["href","rel","target","tag"]))}}),Wv=be(Gv,[["__scopeId","data-v-a3976bdc"]]),$v={key:0,class:"VPFeatures"},Xv={class:"container"},qv={class:"items"},Yv=Ce({__name:"VPFeatures",props:{features:{}},setup(i){const e=i,t=qe(()=>{const n=e.features.length;if(n){if(n===2)return"grid-2";if(n===3)return"grid-3";if(n%3===0)return"grid-6";if(n>3)return"grid-4"}else return});return(n,s)=>n.features?(L(),F("div",$v,[w("div",Xv,[w("div",qv,[(L(!0),F(tt,null,vt(n.features,r=>(L(),F("div",{key:r.title,class:ct(["item",[t.value]])},[Ie(Wv,{icon:r.icon,title:r.title,details:r.details,link:r.link,"link-text":r.linkText,rel:r.rel,target:r.target},null,8,["icon","title","details","link","link-text","rel","target"])],2))),128))])])])):_e("",!0)}}),jv=be(Yv,[["__scopeId","data-v-a6181336"]]),Zv=Ce({__name:"VPHomeFeatures",setup(i){const{frontmatter:e}=Qe();return(t,n)=>V(e).features?(L(),De(jv,{key:0,class:"VPHomeFeatures",features:V(e).features},null,8,["features"])):_e("",!0)}}),Kv=Ce({__name:"VPHomeContent",setup(i){const{width:e}=Eu({initialWidth:0,includeScrollbar:!1});return(t,n)=>(L(),F("div",{class:"vp-doc container",style:ho(V(e)?{"--vp-offset":`calc(50% - ${V(e)/2}px)`}:{})},[J(t.$slots,"default",{},void 0,!0)],4))}}),Jv=be(Kv,[["__scopeId","data-v-8e2d4988"]]),Qv=Ce({__name:"VPHome",setup(i){const{frontmatter:e,theme:t}=Qe();return(n,s)=>{const r=fi("Content");return L(),F("div",{class:ct(["VPHome",{"external-link-icon-enabled":V(t).externalLinkIcon}])},[J(n.$slots,"home-hero-before",{},void 0,!0),Ie(kv,null,{"home-hero-info-before":te(()=>[J(n.$slots,"home-hero-info-before",{},void 0,!0)]),"home-hero-info":te(()=>[J(n.$slots,"home-hero-info",{},void 0,!0)]),"home-hero-info-after":te(()=>[J(n.$slots,"home-hero-info-after",{},void 0,!0)]),"home-hero-actions-after":te(()=>[J(n.$slots,"home-hero-actions-after",{},void 0,!0)]),"home-hero-image":te(()=>[J(n.$slots,"home-hero-image",{},void 0,!0)]),_:3}),J(n.$slots,"home-hero-after",{},void 0,!0),J(n.$slots,"home-features-before",{},void 0,!0),Ie(Zv),J(n.$slots,"home-features-after",{},void 0,!0),V(e).markdownStyles!==!1?(L(),De(Jv,{key:0},{default:te(()=>[Ie(r)]),_:1})):(L(),De(r,{key:1}))],2)}}}),e0=be(Qv,[["__scopeId","data-v-8b561e3d"]]),t0={},n0={class:"VPPage"};function i0(i,e){const t=fi("Content");return L(),F("div",n0,[J(i.$slots,"page-top"),Ie(t),J(i.$slots,"page-bottom")])}const s0=be(t0,[["render",i0]]),r0=Ce({__name:"VPContent",setup(i){const{page:e,frontmatter:t}=Qe(),{hasSidebar:n}=kn();return(s,r)=>(L(),F("div",{class:ct(["VPContent",{"has-sidebar":V(n),"is-home":V(t).layout==="home"}]),id:"VPContent"},[V(e).isNotFound?J(s.$slots,"not-found",{key:0},()=>[Ie($u)],!0):V(t).layout==="page"?(L(),De(s0,{key:1},{"page-top":te(()=>[J(s.$slots,"page-top",{},void 0,!0)]),"page-bottom":te(()=>[J(s.$slots,"page-bottom",{},void 0,!0)]),_:3})):V(t).layout==="home"?(L(),De(e0,{key:2},{"home-hero-before":te(()=>[J(s.$slots,"home-hero-before",{},void 0,!0)]),"home-hero-info-before":te(()=>[J(s.$slots,"home-hero-info-before",{},void 0,!0)]),"home-hero-info":te(()=>[J(s.$slots,"home-hero-info",{},void 0,!0)]),"home-hero-info-after":te(()=>[J(s.$slots,"home-hero-info-after",{},void 0,!0)]),"home-hero-actions-after":te(()=>[J(s.$slots,"home-hero-actions-after",{},void 0,!0)]),"home-hero-image":te(()=>[J(s.$slots,"home-hero-image",{},void 0,!0)]),"home-hero-after":te(()=>[J(s.$slots,"home-hero-after",{},void 0,!0)]),"home-features-before":te(()=>[J(s.$slots,"home-features-before",{},void 0,!0)]),"home-features-after":te(()=>[J(s.$slots,"home-features-after",{},void 0,!0)]),_:3})):V(t).layout&&V(t).layout!=="doc"?(L(),De(vn(V(t).layout),{key:3})):(L(),De(Vd,{key:4},{"doc-top":te(()=>[J(s.$slots,"doc-top",{},void 0,!0)]),"doc-bottom":te(()=>[J(s.$slots,"doc-bottom",{},void 0,!0)]),"doc-footer-before":te(()=>[J(s.$slots,"doc-footer-before",{},void 0,!0)]),"doc-before":te(()=>[J(s.$slots,"doc-before",{},void 0,!0)]),"doc-after":te(()=>[J(s.$slots,"doc-after",{},void 0,!0)]),"aside-top":te(()=>[J(s.$slots,"aside-top",{},void 0,!0)]),"aside-outline-before":te(()=>[J(s.$slots,"aside-outline-before",{},void 0,!0)]),"aside-outline-after":te(()=>[J(s.$slots,"aside-outline-after",{},void 0,!0)]),"aside-ads-before":te(()=>[J(s.$slots,"aside-ads-before",{},void 0,!0)]),"aside-ads-after":te(()=>[J(s.$slots,"aside-ads-after",{},void 0,!0)]),"aside-bottom":te(()=>[J(s.$slots,"aside-bottom",{},void 0,!0)]),_:3}))],2))}}),a0=be(r0,[["__scopeId","data-v-1428d186"]]),o0={class:"container"},l0=["innerHTML"],c0=["innerHTML"],u0=Ce({__name:"VPFooter",setup(i){const{theme:e,frontmatter:t}=Qe(),{hasSidebar:n}=kn();return(s,r)=>V(e).footer&&V(t).footer!==!1?(L(),F("footer",{key:0,class:ct(["VPFooter",{"has-sidebar":V(n)}])},[w("div",o0,[V(e).footer.message?(L(),F("p",{key:0,class:"message",innerHTML:V(e).footer.message},null,8,l0)):_e("",!0),V(e).footer.copyright?(L(),F("p",{key:1,class:"copyright",innerHTML:V(e).footer.copyright},null,8,c0)):_e("",!0)])],2)):_e("",!0)}}),d0=be(u0,[["__scopeId","data-v-e315a0ad"]]);function h0(){const{theme:i,frontmatter:e}=Qe(),t=cc([]),n=qe(()=>t.value.length>0);return fr(()=>{t.value=mo(e.value.outline??i.value.outline)}),{headers:t,hasLocalNav:n}}const f0={class:"menu-text"},p0={class:"header"},m0={class:"outline"},g0=Ce({__name:"VPLocalNavOutlineDropdown",props:{headers:{},navHeight:{}},setup(i){const e=i,{theme:t}=Qe(),n=We(!1),s=We(0),r=We(),a=We();function o(d){var h;(h=r.value)!=null&&h.contains(d.target)||(n.value=!1)}Un(n,d=>{if(d){document.addEventListener("click",o);return}document.removeEventListener("click",o)}),fa("Escape",()=>{n.value=!1}),fr(()=>{n.value=!1});function l(){n.value=!n.value,s.value=window.innerHeight+Math.min(window.scrollY-e.navHeight,0)}function c(d){d.target.classList.contains("outline-link")&&(a.value&&(a.value.style.transition="none"),dc(()=>{n.value=!1}))}function u(){n.value=!1,window.scrollTo({top:0,left:0,behavior:"smooth"})}return(d,h)=>(L(),F("div",{class:"VPLocalNavOutlineDropdown",style:ho({"--vp-vh":s.value+"px"}),ref_key:"main",ref:r},[d.headers.length>0?(L(),F("button",{key:0,onClick:l,class:ct({open:n.value})},[w("span",f0,xe(V(_c)(V(t))),1),h[0]||(h[0]=w("span",{class:"vpi-chevron-right icon"},null,-1))],2)):(L(),F("button",{key:1,onClick:u},xe(V(t).returnToTopLabel||"Return to top"),1)),Ie(co,{name:"flyout"},{default:te(()=>[n.value?(L(),F("div",{key:0,ref_key:"items",ref:a,class:"items",onClick:c},[w("div",p0,[w("a",{class:"top-link",href:"#",onClick:u},xe(V(t).returnToTopLabel||"Return to top"),1)]),w("div",m0,[Ie(vc,{headers:d.headers},null,8,["headers"])])],512)):_e("",!0)]),_:1})],4))}}),_0=be(g0,[["__scopeId","data-v-8a42e2b4"]]),v0={class:"container"},x0=["aria-expanded"],M0={class:"menu-text"},S0=Ce({__name:"VPLocalNav",props:{open:{type:Boolean}},emits:["open-menu"],setup(i){const{theme:e,frontmatter:t}=Qe(),{hasSidebar:n}=kn(),{headers:s}=h0(),{y:r}=hc(),a=We(0);zt(()=>{a.value=parseInt(getComputedStyle(document.documentElement).getPropertyValue("--vp-nav-height"))}),fr(()=>{s.value=mo(t.value.outline??e.value.outline)});const o=qe(()=>s.value.length===0),l=qe(()=>o.value&&!n.value),c=qe(()=>({VPLocalNav:!0,"has-sidebar":n.value,empty:o.value,fixed:l.value}));return(u,d)=>V(t).layout!=="home"&&(!l.value||V(r)>=a.value)?(L(),F("div",{key:0,class:ct(c.value)},[w("div",v0,[V(n)?(L(),F("button",{key:0,class:"menu","aria-expanded":u.open,"aria-controls":"VPSidebarNav",onClick:d[0]||(d[0]=h=>u.$emit("open-menu"))},[d[1]||(d[1]=w("span",{class:"vpi-align-left menu-icon"},null,-1)),w("span",M0,xe(V(e).sidebarMenuLabel||"Menu"),1)],8,x0)):_e("",!0),Ie(_0,{headers:V(s),navHeight:a.value},null,8,["headers","navHeight"])])],2)):_e("",!0)}}),y0=be(S0,[["__scopeId","data-v-a6f0e41e"]]);function b0(){const i=We(!1);function e(){i.value=!0,window.addEventListener("resize",s)}function t(){i.value=!1,window.removeEventListener("resize",s)}function n(){i.value?t():e()}function s(){window.outerWidth>=768&&t()}const r=pr();return Un(()=>r.path,t),{isScreenOpen:i,openScreen:e,closeScreen:t,toggleScreen:n}}const E0={},T0={class:"VPSwitch",type:"button",role:"switch"},w0={class:"check"},A0={key:0,class:"icon"};function R0(i,e){return L(),F("button",T0,[w("span",w0,[i.$slots.default?(L(),F("span",A0,[J(i.$slots,"default",{},void 0,!0)])):_e("",!0)])])}const C0=be(E0,[["render",R0],["__scopeId","data-v-1d5665e3"]]),P0=Ce({__name:"VPSwitchAppearance",setup(i){const{isDark:e,theme:t}=Qe(),n=mr("toggle-appearance",()=>{e.value=!e.value}),s=We("");return uo(()=>{s.value=e.value?t.value.lightModeSwitchTitle||"Switch to light theme":t.value.darkModeSwitchTitle||"Switch to dark theme"}),(r,a)=>(L(),De(C0,{title:s.value,class:"VPSwitchAppearance","aria-checked":V(e),onClick:V(n)},{default:te(()=>a[0]||(a[0]=[w("span",{class:"vpi-sun sun"},null,-1),w("span",{class:"vpi-moon moon"},null,-1)])),_:1,__:[0]},8,["title","aria-checked","onClick"]))}}),ko=be(P0,[["__scopeId","data-v-5337faa4"]]),L0={key:0,class:"VPNavBarAppearance"},D0=Ce({__name:"VPNavBarAppearance",setup(i){const{site:e}=Qe();return(t,n)=>V(e).appearance&&V(e).appearance!=="force-dark"&&V(e).appearance!=="force-auto"?(L(),F("div",L0,[Ie(ko)])):_e("",!0)}}),I0=be(D0,[["__scopeId","data-v-6c893767"]]),No=We();let Kc=!1,oa=0;function U0(i){const e=We(!1);if(gr){!Kc&&k0(),oa++;const t=Un(No,n=>{var s,r,a;n===i.el.value||(s=i.el.value)!=null&&s.contains(n)?(e.value=!0,(r=i.onFocus)==null||r.call(i)):(e.value=!1,(a=i.onBlur)==null||a.call(i))});jn(()=>{t(),oa--,oa||N0()})}return Tu(e)}function k0(){document.addEventListener("focusin",Jc),Kc=!0,No.value=document.activeElement}function N0(){document.removeEventListener("focusin",Jc)}function Jc(){No.value=document.activeElement}const F0={class:"VPMenuLink"},O0=["innerHTML"],B0=Ce({__name:"VPMenuLink",props:{item:{}},setup(i){const{page:e}=Qe();return(t,n)=>(L(),F("div",F0,[Ie(xn,{class:ct({active:V(ci)(V(e).relativePath,t.item.activeMatch||t.item.link,!!t.item.activeMatch)}),href:t.item.link,target:t.item.target,rel:t.item.rel,"no-icon":t.item.noIcon},{default:te(()=>[w("span",{innerHTML:t.item.text},null,8,O0)]),_:1},8,["class","href","target","rel","no-icon"])]))}}),br=be(B0,[["__scopeId","data-v-35975db6"]]),z0={class:"VPMenuGroup"},V0={key:0,class:"title"},H0=Ce({__name:"VPMenuGroup",props:{text:{},items:{}},setup(i){return(e,t)=>(L(),F("div",z0,[e.text?(L(),F("p",V0,xe(e.text),1)):_e("",!0),(L(!0),F(tt,null,vt(e.items,n=>(L(),F(tt,null,["link"in n?(L(),De(br,{key:0,item:n},null,8,["item"])):_e("",!0)],64))),256))]))}}),G0=be(H0,[["__scopeId","data-v-69e747b5"]]),W0={class:"VPMenu"},$0={key:0,class:"items"},X0=Ce({__name:"VPMenu",props:{items:{}},setup(i){return(e,t)=>(L(),F("div",W0,[e.items?(L(),F("div",$0,[(L(!0),F(tt,null,vt(e.items,n=>(L(),F(tt,{key:JSON.stringify(n)},["link"in n?(L(),De(br,{key:0,item:n},null,8,["item"])):"component"in n?(L(),De(vn(n.component),Gn({key:1,ref_for:!0},n.props),null,16)):(L(),De(G0,{key:2,text:n.text,items:n.items},null,8,["text","items"]))],64))),128))])):_e("",!0),J(e.$slots,"default",{},void 0,!0)]))}}),q0=be(X0,[["__scopeId","data-v-b98bc113"]]),Y0=["aria-expanded","aria-label"],j0={key:0,class:"text"},Z0=["innerHTML"],K0={key:1,class:"vpi-more-horizontal icon"},J0={class:"menu"},Q0=Ce({__name:"VPFlyout",props:{icon:{},button:{},label:{},items:{}},setup(i){const e=We(!1),t=We();U0({el:t,onBlur:n});function n(){e.value=!1}return(s,r)=>(L(),F("div",{class:"VPFlyout",ref_key:"el",ref:t,onMouseenter:r[1]||(r[1]=a=>e.value=!0),onMouseleave:r[2]||(r[2]=a=>e.value=!1)},[w("button",{type:"button",class:"button","aria-haspopup":"true","aria-expanded":e.value,"aria-label":s.label,onClick:r[0]||(r[0]=a=>e.value=!e.value)},[s.button||s.icon?(L(),F("span",j0,[s.icon?(L(),F("span",{key:0,class:ct([s.icon,"option-icon"])},null,2)):_e("",!0),s.button?(L(),F("span",{key:1,innerHTML:s.button},null,8,Z0)):_e("",!0),r[3]||(r[3]=w("span",{class:"vpi-chevron-down text-icon"},null,-1))])):(L(),F("span",K0))],8,Y0),w("div",J0,[Ie(q0,{items:s.items},{default:te(()=>[J(s.$slots,"default",{},void 0,!0)]),_:3},8,["items"])])],544))}}),Fo=be(Q0,[["__scopeId","data-v-cf11d7a2"]]),ex=["href","aria-label","innerHTML"],tx=Ce({__name:"VPSocialLink",props:{icon:{},link:{},ariaLabel:{}},setup(i){const e=i,t=We();zt(async()=>{var r;await dc();const s=(r=t.value)==null?void 0:r.children[0];s instanceof HTMLElement&&s.className.startsWith("vpi-social-")&&(getComputedStyle(s).maskImage||getComputedStyle(s).webkitMaskImage)==="none"&&s.style.setProperty("--icon",`url('https://api.iconify.design/simple-icons/${e.icon}.svg')`)});const n=qe(()=>typeof e.icon=="object"?e.icon.svg:`<span class="vpi-social-${e.icon}"></span>`);return(s,r)=>(L(),F("a",{ref_key:"el",ref:t,class:"VPSocialLink no-icon",href:s.link,"aria-label":s.ariaLabel??(typeof s.icon=="string"?s.icon:""),target:"_blank",rel:"noopener",innerHTML:n.value},null,8,ex))}}),nx=be(tx,[["__scopeId","data-v-bd121fe5"]]),ix={class:"VPSocialLinks"},sx=Ce({__name:"VPSocialLinks",props:{links:{}},setup(i){return(e,t)=>(L(),F("div",ix,[(L(!0),F(tt,null,vt(e.links,({link:n,icon:s,ariaLabel:r})=>(L(),De(nx,{key:n,icon:s,link:n,ariaLabel:r},null,8,["icon","link","ariaLabel"]))),128))]))}}),Oo=be(sx,[["__scopeId","data-v-7bc22406"]]),rx={key:0,class:"group translations"},ax={class:"trans-title"},ox={key:1,class:"group"},lx={class:"item appearance"},cx={class:"label"},ux={class:"appearance-action"},dx={key:2,class:"group"},hx={class:"item social-links"},fx=Ce({__name:"VPNavBarExtra",setup(i){const{site:e,theme:t}=Qe(),{localeLinks:n,currentLang:s}=fs({correspondingLink:!0}),r=qe(()=>n.value.length&&s.value.label||e.value.appearance||t.value.socialLinks);return(a,o)=>r.value?(L(),De(Fo,{key:0,class:"VPNavBarExtra",label:"extra navigation"},{default:te(()=>[V(n).length&&V(s).label?(L(),F("div",rx,[w("p",ax,xe(V(s).label),1),(L(!0),F(tt,null,vt(V(n),l=>(L(),De(br,{key:l.link,item:l},null,8,["item"]))),128))])):_e("",!0),V(e).appearance&&V(e).appearance!=="force-dark"&&V(e).appearance!=="force-auto"?(L(),F("div",ox,[w("div",lx,[w("p",cx,xe(V(t).darkModeSwitchLabel||"Appearance"),1),w("div",ux,[Ie(ko)])])])):_e("",!0),V(t).socialLinks?(L(),F("div",dx,[w("div",hx,[Ie(Oo,{class:"social-links-list",links:V(t).socialLinks},null,8,["links"])])])):_e("",!0)]),_:1})):_e("",!0)}}),px=be(fx,[["__scopeId","data-v-bb2aa2f0"]]),mx=["aria-expanded"],gx=Ce({__name:"VPNavBarHamburger",props:{active:{type:Boolean}},emits:["click"],setup(i){return(e,t)=>(L(),F("button",{type:"button",class:ct(["VPNavBarHamburger",{active:e.active}]),"aria-label":"mobile navigation","aria-expanded":e.active,"aria-controls":"VPNavScreen",onClick:t[0]||(t[0]=n=>e.$emit("click"))},t[1]||(t[1]=[w("span",{class:"container"},[w("span",{class:"top"}),w("span",{class:"middle"}),w("span",{class:"bottom"})],-1)]),10,mx))}}),_x=be(gx,[["__scopeId","data-v-e5dd9c1c"]]),vx=["innerHTML"],xx=Ce({__name:"VPNavBarMenuLink",props:{item:{}},setup(i){const{page:e}=Qe();return(t,n)=>(L(),De(xn,{class:ct({VPNavBarMenuLink:!0,active:V(ci)(V(e).relativePath,t.item.activeMatch||t.item.link,!!t.item.activeMatch)}),href:t.item.link,target:t.item.target,rel:t.item.rel,"no-icon":t.item.noIcon,tabindex:"0"},{default:te(()=>[w("span",{innerHTML:t.item.text},null,8,vx)]),_:1},8,["class","href","target","rel","no-icon"]))}}),Mx=be(xx,[["__scopeId","data-v-e56f3d57"]]),Sx=Ce({__name:"VPNavBarMenuGroup",props:{item:{}},setup(i){const e=i,{page:t}=Qe(),n=r=>"component"in r?!1:"link"in r?ci(t.value.relativePath,r.link,!!e.item.activeMatch):r.items.some(n),s=qe(()=>n(e.item));return(r,a)=>(L(),De(Fo,{class:ct({VPNavBarMenuGroup:!0,active:V(ci)(V(t).relativePath,r.item.activeMatch,!!r.item.activeMatch)||s.value}),button:r.item.text,items:r.item.items},null,8,["class","button","items"]))}}),yx={key:0,"aria-labelledby":"main-nav-aria-label",class:"VPNavBarMenu"},bx=Ce({__name:"VPNavBarMenu",setup(i){const{theme:e}=Qe();return(t,n)=>V(e).nav?(L(),F("nav",yx,[n[0]||(n[0]=w("span",{id:"main-nav-aria-label",class:"visually-hidden"}," Main Navigation ",-1)),(L(!0),F(tt,null,vt(V(e).nav,s=>(L(),F(tt,{key:JSON.stringify(s)},["link"in s?(L(),De(Mx,{key:0,item:s},null,8,["item"])):"component"in s?(L(),De(vn(s.component),Gn({key:1,ref_for:!0},s.props),null,16)):(L(),De(Sx,{key:2,item:s},null,8,["item"]))],64))),128))])):_e("",!0)}}),Ex=be(bx,[["__scopeId","data-v-dc692963"]]);function Tx(i){const{localeIndex:e,theme:t}=Qe();function n(s){var g,_,m;const r=s.split("."),a=(g=t.value.search)==null?void 0:g.options,o=a&&typeof a=="object",l=o&&((m=(_=a.locales)==null?void 0:_[e.value])==null?void 0:m.translations)||null,c=o&&a.translations||null;let u=l,d=c,h=i;const p=r.pop();for(const f of r){let E=null;const b=h==null?void 0:h[f];b&&(E=h=b);const v=d==null?void 0:d[f];v&&(E=d=v);const T=u==null?void 0:u[f];T&&(E=u=T),b||(h=E),v||(d=E),T||(u=E)}return(u==null?void 0:u[p])??(d==null?void 0:d[p])??(h==null?void 0:h[p])??""}return n}const wx=["aria-label"],Ax={class:"DocSearch-Button-Container"},Rx={class:"DocSearch-Button-Placeholder"},ic=Ce({__name:"VPNavBarSearchButton",setup(i){const t=Tx({button:{buttonText:"Search",buttonAriaLabel:"Search"}});return(n,s)=>(L(),F("button",{type:"button",class:"DocSearch DocSearch-Button","aria-label":V(t)("button.buttonAriaLabel")},[w("span",Ax,[s[0]||(s[0]=w("span",{class:"vp-icon DocSearch-Search-Icon"},null,-1)),w("span",Rx,xe(V(t)("button.buttonText")),1)]),s[1]||(s[1]=w("span",{class:"DocSearch-Button-Keys"},[w("kbd",{class:"DocSearch-Button-Key"}),w("kbd",{class:"DocSearch-Button-Key"},"K")],-1))],8,wx))}}),Cx={class:"VPNavBarSearch"},Px={id:"local-search"},Lx={key:1,id:"docsearch"},Dx=Ce({__name:"VPNavBarSearch",setup(i){const e=wu(()=>Au(()=>import("./VPLocalSearchBox.-wtWpN5J.js"),__vite__mapDeps([0,1]))),t=()=>null,{theme:n}=Qe(),s=We(!1),r=We(!1);zt(()=>{});function a(){s.value||(s.value=!0,setTimeout(o,16))}function o(){const d=new Event("keydown");d.key="k",d.metaKey=!0,window.dispatchEvent(d),setTimeout(()=>{document.querySelector(".DocSearch-Modal")||o()},16)}function l(d){const h=d.target,p=h.tagName;return h.isContentEditable||p==="INPUT"||p==="SELECT"||p==="TEXTAREA"}const c=We(!1);fa("k",d=>{(d.ctrlKey||d.metaKey)&&(d.preventDefault(),c.value=!0)}),fa("/",d=>{l(d)||(d.preventDefault(),c.value=!0)});const u="local";return(d,h)=>{var p;return L(),F("div",Cx,[V(u)==="local"?(L(),F(tt,{key:0},[c.value?(L(),De(V(e),{key:0,onClose:h[0]||(h[0]=g=>c.value=!1)})):_e("",!0),w("div",Px,[Ie(ic,{onClick:h[1]||(h[1]=g=>c.value=!0)})])],64)):V(u)==="algolia"?(L(),F(tt,{key:1},[s.value?(L(),De(V(t),{key:0,algolia:((p=V(n).search)==null?void 0:p.options)??V(n).algolia,onVnodeBeforeMount:h[2]||(h[2]=g=>r.value=!0)},null,8,["algolia"])):_e("",!0),r.value?_e("",!0):(L(),F("div",Lx,[Ie(ic,{onClick:a})]))],64)):_e("",!0)])}}}),Ix=Ce({__name:"VPNavBarSocialLinks",setup(i){const{theme:e}=Qe();return(t,n)=>V(e).socialLinks?(L(),De(Oo,{key:0,class:"VPNavBarSocialLinks",links:V(e).socialLinks},null,8,["links"])):_e("",!0)}}),Ux=be(Ix,[["__scopeId","data-v-0394ad82"]]),kx=["href","rel","target"],Nx=["innerHTML"],Fx={key:2},Ox=Ce({__name:"VPNavBarTitle",setup(i){const{site:e,theme:t}=Qe(),{hasSidebar:n}=kn(),{currentLang:s}=fs(),r=qe(()=>{var l;return typeof t.value.logoLink=="string"?t.value.logoLink:(l=t.value.logoLink)==null?void 0:l.link}),a=qe(()=>{var l;return typeof t.value.logoLink=="string"||(l=t.value.logoLink)==null?void 0:l.rel}),o=qe(()=>{var l;return typeof t.value.logoLink=="string"||(l=t.value.logoLink)==null?void 0:l.target});return(l,c)=>(L(),F("div",{class:ct(["VPNavBarTitle",{"has-sidebar":V(n)}])},[w("a",{class:"title",href:r.value??V(po)(V(s).link),rel:a.value,target:o.value},[J(l.$slots,"nav-bar-title-before",{},void 0,!0),V(t).logo?(L(),De(ir,{key:0,class:"logo",image:V(t).logo},null,8,["image"])):_e("",!0),V(t).siteTitle?(L(),F("span",{key:1,innerHTML:V(t).siteTitle},null,8,Nx)):V(t).siteTitle===void 0?(L(),F("span",Fx,xe(V(e).title),1)):_e("",!0),J(l.$slots,"nav-bar-title-after",{},void 0,!0)],8,kx)],2))}}),Bx=be(Ox,[["__scopeId","data-v-1168a8e4"]]),zx={class:"items"},Vx={class:"title"},Hx=Ce({__name:"VPNavBarTranslations",setup(i){const{theme:e}=Qe(),{localeLinks:t,currentLang:n}=fs({correspondingLink:!0});return(s,r)=>V(t).length&&V(n).label?(L(),De(Fo,{key:0,class:"VPNavBarTranslations",icon:"vpi-languages",label:V(e).langMenuLabel||"Change language"},{default:te(()=>[w("div",zx,[w("p",Vx,xe(V(n).label),1),(L(!0),F(tt,null,vt(V(t),a=>(L(),De(br,{key:a.link,item:a},null,8,["item"]))),128))])]),_:1},8,["label"])):_e("",!0)}}),Gx=be(Hx,[["__scopeId","data-v-88af2de4"]]),Wx={class:"wrapper"},$x={class:"container"},Xx={class:"title"},qx={class:"content"},Yx={class:"content-body"},jx=Ce({__name:"VPNavBar",props:{isScreenOpen:{type:Boolean}},emits:["toggle-screen"],setup(i){const e=i,{y:t}=hc(),{hasSidebar:n}=kn(),{frontmatter:s}=Qe(),r=We({});return uo(()=>{r.value={"has-sidebar":n.value,home:s.value.layout==="home",top:t.value===0,"screen-open":e.isScreenOpen}}),(a,o)=>(L(),F("div",{class:ct(["VPNavBar",r.value])},[w("div",Wx,[w("div",$x,[w("div",Xx,[Ie(Bx,null,{"nav-bar-title-before":te(()=>[J(a.$slots,"nav-bar-title-before",{},void 0,!0)]),"nav-bar-title-after":te(()=>[J(a.$slots,"nav-bar-title-after",{},void 0,!0)]),_:3})]),w("div",qx,[w("div",Yx,[J(a.$slots,"nav-bar-content-before",{},void 0,!0),Ie(Dx,{class:"search"}),Ie(Ex,{class:"menu"}),Ie(Gx,{class:"translations"}),Ie(I0,{class:"appearance"}),Ie(Ux,{class:"social-links"}),Ie(px,{class:"extra"}),J(a.$slots,"nav-bar-content-after",{},void 0,!0),Ie(_x,{class:"hamburger",active:a.isScreenOpen,onClick:o[0]||(o[0]=l=>a.$emit("toggle-screen"))},null,8,["active"])])])])]),o[1]||(o[1]=w("div",{class:"divider"},[w("div",{class:"divider-line"})],-1))],2))}}),Zx=be(jx,[["__scopeId","data-v-6aa21345"]]),Kx={key:0,class:"VPNavScreenAppearance"},Jx={class:"text"},Qx=Ce({__name:"VPNavScreenAppearance",setup(i){const{site:e,theme:t}=Qe();return(n,s)=>V(e).appearance&&V(e).appearance!=="force-dark"&&V(e).appearance!=="force-auto"?(L(),F("div",Kx,[w("p",Jx,xe(V(t).darkModeSwitchLabel||"Appearance"),1),Ie(ko)])):_e("",!0)}}),eM=be(Qx,[["__scopeId","data-v-b44890b2"]]),tM=["innerHTML"],nM=Ce({__name:"VPNavScreenMenuLink",props:{item:{}},setup(i){const e=mr("close-screen");return(t,n)=>(L(),De(xn,{class:"VPNavScreenMenuLink",href:t.item.link,target:t.item.target,rel:t.item.rel,"no-icon":t.item.noIcon,onClick:V(e)},{default:te(()=>[w("span",{innerHTML:t.item.text},null,8,tM)]),_:1},8,["href","target","rel","no-icon","onClick"]))}}),iM=be(nM,[["__scopeId","data-v-df37e6dd"]]),sM=["innerHTML"],rM=Ce({__name:"VPNavScreenMenuGroupLink",props:{item:{}},setup(i){const e=mr("close-screen");return(t,n)=>(L(),De(xn,{class:"VPNavScreenMenuGroupLink",href:t.item.link,target:t.item.target,rel:t.item.rel,"no-icon":t.item.noIcon,onClick:V(e)},{default:te(()=>[w("span",{innerHTML:t.item.text},null,8,sM)]),_:1},8,["href","target","rel","no-icon","onClick"]))}}),Qc=be(rM,[["__scopeId","data-v-3e9c20e4"]]),aM={class:"VPNavScreenMenuGroupSection"},oM={key:0,class:"title"},lM=Ce({__name:"VPNavScreenMenuGroupSection",props:{text:{},items:{}},setup(i){return(e,t)=>(L(),F("div",aM,[e.text?(L(),F("p",oM,xe(e.text),1)):_e("",!0),(L(!0),F(tt,null,vt(e.items,n=>(L(),De(Qc,{key:n.text,item:n},null,8,["item"]))),128))]))}}),cM=be(lM,[["__scopeId","data-v-8133b170"]]),uM=["aria-controls","aria-expanded"],dM=["innerHTML"],hM=["id"],fM={key:0,class:"item"},pM={key:1,class:"item"},mM={key:2,class:"group"},gM=Ce({__name:"VPNavScreenMenuGroup",props:{text:{},items:{}},setup(i){const e=i,t=We(!1),n=qe(()=>`NavScreenGroup-${e.text.replace(" ","-").toLowerCase()}`);function s(){t.value=!t.value}return(r,a)=>(L(),F("div",{class:ct(["VPNavScreenMenuGroup",{open:t.value}])},[w("button",{class:"button","aria-controls":n.value,"aria-expanded":t.value,onClick:s},[w("span",{class:"button-text",innerHTML:r.text},null,8,dM),a[0]||(a[0]=w("span",{class:"vpi-plus button-icon"},null,-1))],8,uM),w("div",{id:n.value,class:"items"},[(L(!0),F(tt,null,vt(r.items,o=>(L(),F(tt,{key:JSON.stringify(o)},["link"in o?(L(),F("div",fM,[Ie(Qc,{item:o},null,8,["item"])])):"component"in o?(L(),F("div",pM,[(L(),De(vn(o.component),Gn({ref_for:!0},o.props,{"screen-menu":""}),null,16))])):(L(),F("div",mM,[Ie(cM,{text:o.text,items:o.items},null,8,["text","items"])]))],64))),128))],8,hM)],2))}}),_M=be(gM,[["__scopeId","data-v-b9ab8c58"]]),vM={key:0,class:"VPNavScreenMenu"},xM=Ce({__name:"VPNavScreenMenu",setup(i){const{theme:e}=Qe();return(t,n)=>V(e).nav?(L(),F("nav",vM,[(L(!0),F(tt,null,vt(V(e).nav,s=>(L(),F(tt,{key:JSON.stringify(s)},["link"in s?(L(),De(iM,{key:0,item:s},null,8,["item"])):"component"in s?(L(),De(vn(s.component),Gn({key:1,ref_for:!0},s.props,{"screen-menu":""}),null,16)):(L(),De(_M,{key:2,text:s.text||"",items:s.items},null,8,["text","items"]))],64))),128))])):_e("",!0)}}),MM=Ce({__name:"VPNavScreenSocialLinks",setup(i){const{theme:e}=Qe();return(t,n)=>V(e).socialLinks?(L(),De(Oo,{key:0,class:"VPNavScreenSocialLinks",links:V(e).socialLinks},null,8,["links"])):_e("",!0)}}),SM={class:"list"},yM=Ce({__name:"VPNavScreenTranslations",setup(i){const{localeLinks:e,currentLang:t}=fs({correspondingLink:!0}),n=We(!1);function s(){n.value=!n.value}return(r,a)=>V(e).length&&V(t).label?(L(),F("div",{key:0,class:ct(["VPNavScreenTranslations",{open:n.value}])},[w("button",{class:"title",onClick:s},[a[0]||(a[0]=w("span",{class:"vpi-languages icon lang"},null,-1)),Xn(" "+xe(V(t).label)+" ",1),a[1]||(a[1]=w("span",{class:"vpi-chevron-down icon chevron"},null,-1))]),w("ul",SM,[(L(!0),F(tt,null,vt(V(e),o=>(L(),F("li",{key:o.link,class:"item"},[Ie(xn,{class:"link",href:o.link},{default:te(()=>[Xn(xe(o.text),1)]),_:2},1032,["href"])]))),128))])],2)):_e("",!0)}}),bM=be(yM,[["__scopeId","data-v-858fe1a4"]]),EM={class:"container"},TM=Ce({__name:"VPNavScreen",props:{open:{type:Boolean}},setup(i){const e=We(null),t=fc(gr?document.body:null);return(n,s)=>(L(),De(co,{name:"fade",onEnter:s[0]||(s[0]=r=>t.value=!0),onAfterLeave:s[1]||(s[1]=r=>t.value=!1)},{default:te(()=>[n.open?(L(),F("div",{key:0,class:"VPNavScreen",ref_key:"screen",ref:e,id:"VPNavScreen"},[w("div",EM,[J(n.$slots,"nav-screen-content-before",{},void 0,!0),Ie(xM,{class:"menu"}),Ie(bM,{class:"translations"}),Ie(eM,{class:"appearance"}),Ie(MM,{class:"social-links"}),J(n.$slots,"nav-screen-content-after",{},void 0,!0)])],512)):_e("",!0)]),_:3}))}}),wM=be(TM,[["__scopeId","data-v-f2779853"]]),AM={key:0,class:"VPNav"},RM=Ce({__name:"VPNav",setup(i){const{isScreenOpen:e,closeScreen:t,toggleScreen:n}=b0(),{frontmatter:s}=Qe(),r=qe(()=>s.value.navbar!==!1);return pc("close-screen",t),hr(()=>{gr&&document.documentElement.classList.toggle("hide-nav",!r.value)}),(a,o)=>r.value?(L(),F("header",AM,[Ie(Zx,{"is-screen-open":V(e),onToggleScreen:V(n)},{"nav-bar-title-before":te(()=>[J(a.$slots,"nav-bar-title-before",{},void 0,!0)]),"nav-bar-title-after":te(()=>[J(a.$slots,"nav-bar-title-after",{},void 0,!0)]),"nav-bar-content-before":te(()=>[J(a.$slots,"nav-bar-content-before",{},void 0,!0)]),"nav-bar-content-after":te(()=>[J(a.$slots,"nav-bar-content-after",{},void 0,!0)]),_:3},8,["is-screen-open","onToggleScreen"]),Ie(wM,{open:V(e)},{"nav-screen-content-before":te(()=>[J(a.$slots,"nav-screen-content-before",{},void 0,!0)]),"nav-screen-content-after":te(()=>[J(a.$slots,"nav-screen-content-after",{},void 0,!0)]),_:3},8,["open"])])):_e("",!0)}}),CM=be(RM,[["__scopeId","data-v-ae24b3ad"]]),PM=["role","tabindex"],LM={key:1,class:"items"},DM=Ce({__name:"VPSidebarItem",props:{item:{},depth:{}},setup(i){const e=i,{collapsed:t,collapsible:n,isLink:s,isActiveLink:r,hasActiveLink:a,hasChildren:o,toggle:l}=ju(qe(()=>e.item)),c=qe(()=>o.value?"section":"div"),u=qe(()=>s.value?"a":"div"),d=qe(()=>o.value?e.depth+2===7?"p":`h${e.depth+2}`:"p"),h=qe(()=>s.value?void 0:"button"),p=qe(()=>[[`level-${e.depth}`],{collapsible:n.value},{collapsed:t.value},{"is-link":s.value},{"is-active":r.value},{"has-active":a.value}]);function g(m){"key"in m&&m.key!=="Enter"||!e.item.link&&l()}function _(){e.item.link&&l()}return(m,f)=>{const E=fi("VPSidebarItem",!0);return L(),De(vn(c.value),{class:ct(["VPSidebarItem",p.value])},{default:te(()=>[m.item.text?(L(),F("div",Gn({key:0,class:"item",role:h.value},Ru(m.item.items?{click:g,keydown:g}:{},!0),{tabindex:m.item.items&&0}),[f[1]||(f[1]=w("div",{class:"indicator"},null,-1)),m.item.link?(L(),De(xn,{key:0,tag:u.value,class:"link",href:m.item.link,rel:m.item.rel,target:m.item.target},{default:te(()=>[(L(),De(vn(d.value),{class:"text",innerHTML:m.item.text},null,8,["innerHTML"]))]),_:1},8,["tag","href","rel","target"])):(L(),De(vn(d.value),{key:1,class:"text",innerHTML:m.item.text},null,8,["innerHTML"])),m.item.collapsed!=null&&m.item.items&&m.item.items.length?(L(),F("div",{key:2,class:"caret",role:"button","aria-label":"toggle section",onClick:_,onKeydown:Cu(_,["enter"]),tabindex:"0"},f[0]||(f[0]=[w("span",{class:"vpi-chevron-right caret-icon"},null,-1)]),32)):_e("",!0)],16,PM)):_e("",!0),m.item.items&&m.item.items.length?(L(),F("div",LM,[m.depth<5?(L(!0),F(tt,{key:0},vt(m.item.items,b=>(L(),De(E,{key:b.text,item:b,depth:m.depth+1},null,8,["item","depth"]))),128)):_e("",!0)])):_e("",!0)]),_:1},8,["class"])}}}),IM=be(DM,[["__scopeId","data-v-b3fd67f8"]]),UM=Ce({__name:"VPSidebarGroup",props:{items:{}},setup(i){const e=We(!0);let t=null;return zt(()=>{t=setTimeout(()=>{t=null,e.value=!1},300)}),ha(()=>{t!=null&&(clearTimeout(t),t=null)}),(n,s)=>(L(!0),F(tt,null,vt(n.items,r=>(L(),F("div",{key:r.text,class:ct(["group",{"no-transition":e.value}])},[Ie(IM,{item:r,depth:0},null,8,["item"])],2))),128))}}),kM=be(UM,[["__scopeId","data-v-c40bc020"]]),NM={class:"nav",id:"VPSidebarNav","aria-labelledby":"sidebar-aria-label",tabindex:"-1"},FM=Ce({__name:"VPSidebar",props:{open:{type:Boolean}},setup(i){const{sidebarGroups:e,hasSidebar:t}=kn(),n=i,s=We(null),r=fc(gr?document.body:null);Un([n,s],()=>{var o;n.open?(r.value=!0,(o=s.value)==null||o.focus()):r.value=!1},{immediate:!0,flush:"post"});const a=We(0);return Un(e,()=>{a.value+=1},{deep:!0}),(o,l)=>V(t)?(L(),F("aside",{key:0,class:ct(["VPSidebar",{open:o.open}]),ref_key:"navEl",ref:s,onClick:l[0]||(l[0]=Pu(()=>{},["stop"]))},[l[2]||(l[2]=w("div",{class:"curtain"},null,-1)),w("nav",NM,[l[1]||(l[1]=w("span",{class:"visually-hidden",id:"sidebar-aria-label"}," Sidebar Navigation ",-1)),J(o.$slots,"sidebar-nav-before",{},void 0,!0),(L(),De(kM,{items:V(e),key:a.value},null,8,["items"])),J(o.$slots,"sidebar-nav-after",{},void 0,!0)])],2)):_e("",!0)}}),OM=be(FM,[["__scopeId","data-v-319d5ca6"]]),BM=Ce({__name:"VPSkipLink",setup(i){const{theme:e}=Qe(),t=pr(),n=We();Un(()=>t.path,()=>n.value.focus());function s({target:r}){const a=document.getElementById(decodeURIComponent(r.hash).slice(1));if(a){const o=()=>{a.removeAttribute("tabindex"),a.removeEventListener("blur",o)};a.setAttribute("tabindex","-1"),a.addEventListener("blur",o),a.focus(),window.scrollTo(0,0)}}return(r,a)=>(L(),F(tt,null,[w("span",{ref_key:"backToTop",ref:n,tabindex:"-1"},null,512),w("a",{href:"#VPContent",class:"VPSkipLink visually-hidden",onClick:s},xe(V(e).skipToContentLabel||"Skip to content"),1)],64))}}),zM=be(BM,[["__scopeId","data-v-0b0ada53"]]),VM=Ce({__name:"Layout",setup(i){const{isOpen:e,open:t,close:n}=kn(),s=pr();Un(()=>s.path,n),Yu(e,n);const{frontmatter:r}=Qe(),a=Lu(),o=qe(()=>!!a["home-hero-image"]);return pc("hero-image-slot-exists",o),(l,c)=>{const u=fi("Content");return V(r).layout!==!1?(L(),F("div",{key:0,class:ct(["Layout",V(r).pageClass])},[J(l.$slots,"layout-top",{},void 0,!0),Ie(zM),Ie(ku,{class:"backdrop",show:V(e),onClick:V(n)},null,8,["show","onClick"]),Ie(CM,null,{"nav-bar-title-before":te(()=>[J(l.$slots,"nav-bar-title-before",{},void 0,!0)]),"nav-bar-title-after":te(()=>[J(l.$slots,"nav-bar-title-after",{},void 0,!0)]),"nav-bar-content-before":te(()=>[J(l.$slots,"nav-bar-content-before",{},void 0,!0)]),"nav-bar-content-after":te(()=>[J(l.$slots,"nav-bar-content-after",{},void 0,!0)]),"nav-screen-content-before":te(()=>[J(l.$slots,"nav-screen-content-before",{},void 0,!0)]),"nav-screen-content-after":te(()=>[J(l.$slots,"nav-screen-content-after",{},void 0,!0)]),_:3}),Ie(y0,{open:V(e),onOpenMenu:V(t)},null,8,["open","onOpenMenu"]),Ie(OM,{open:V(e)},{"sidebar-nav-before":te(()=>[J(l.$slots,"sidebar-nav-before",{},void 0,!0)]),"sidebar-nav-after":te(()=>[J(l.$slots,"sidebar-nav-after",{},void 0,!0)]),_:3},8,["open"]),Ie(a0,null,{"page-top":te(()=>[J(l.$slots,"page-top",{},void 0,!0)]),"page-bottom":te(()=>[J(l.$slots,"page-bottom",{},void 0,!0)]),"not-found":te(()=>[J(l.$slots,"not-found",{},void 0,!0)]),"home-hero-before":te(()=>[J(l.$slots,"home-hero-before",{},void 0,!0)]),"home-hero-info-before":te(()=>[J(l.$slots,"home-hero-info-before",{},void 0,!0)]),"home-hero-info":te(()=>[J(l.$slots,"home-hero-info",{},void 0,!0)]),"home-hero-info-after":te(()=>[J(l.$slots,"home-hero-info-after",{},void 0,!0)]),"home-hero-actions-after":te(()=>[J(l.$slots,"home-hero-actions-after",{},void 0,!0)]),"home-hero-image":te(()=>[J(l.$slots,"home-hero-image",{},void 0,!0)]),"home-hero-after":te(()=>[J(l.$slots,"home-hero-after",{},void 0,!0)]),"home-features-before":te(()=>[J(l.$slots,"home-features-before",{},void 0,!0)]),"home-features-after":te(()=>[J(l.$slots,"home-features-after",{},void 0,!0)]),"doc-footer-before":te(()=>[J(l.$slots,"doc-footer-before",{},void 0,!0)]),"doc-before":te(()=>[J(l.$slots,"doc-before",{},void 0,!0)]),"doc-after":te(()=>[J(l.$slots,"doc-after",{},void 0,!0)]),"doc-top":te(()=>[J(l.$slots,"doc-top",{},void 0,!0)]),"doc-bottom":te(()=>[J(l.$slots,"doc-bottom",{},void 0,!0)]),"aside-top":te(()=>[J(l.$slots,"aside-top",{},void 0,!0)]),"aside-bottom":te(()=>[J(l.$slots,"aside-bottom",{},void 0,!0)]),"aside-outline-before":te(()=>[J(l.$slots,"aside-outline-before",{},void 0,!0)]),"aside-outline-after":te(()=>[J(l.$slots,"aside-outline-after",{},void 0,!0)]),"aside-ads-before":te(()=>[J(l.$slots,"aside-ads-before",{},void 0,!0)]),"aside-ads-after":te(()=>[J(l.$slots,"aside-ads-after",{},void 0,!0)]),_:3}),Ie(d0),J(l.$slots,"layout-bottom",{},void 0,!0)],2)):(L(),De(u,{key:1}))}}}),HM=be(VM,[["__scopeId","data-v-5d98c3a5"]]),GM={Layout:HM,enhanceApp:({app:i})=>{i.component("Badge",Du)}};function Bo(){return{async:!1,breaks:!1,extensions:null,gfm:!0,hooks:null,pedantic:!1,renderer:null,silent:!1,tokenizer:null,walkTokens:null}}var mi=Bo();function eu(i){mi=i}var rs={exec:()=>null};function dt(i,e=""){let t=typeof i=="string"?i:i.source,n={replace:(s,r)=>{let a=typeof r=="string"?r:r.source;return a=a.replace($t.caret,"$1"),t=t.replace(s,a),n},getRegex:()=>new RegExp(t,e)};return n}var $t={codeRemoveIndent:/^(?: {1,4}| {0,3}\t)/gm,outputLinkReplace:/\\([\[\]])/g,indentCodeCompensation:/^(\s+)(?:```)/,beginningSpace:/^\s+/,endingHash:/#$/,startingSpaceChar:/^ /,endingSpaceChar:/ $/,nonSpaceChar:/[^ ]/,newLineCharGlobal:/\n/g,tabCharGlobal:/\t/g,multipleSpaceGlobal:/\s+/g,blankLine:/^[ \t]*$/,doubleBlankLine:/\n[ \t]*\n[ \t]*$/,blockquoteStart:/^ {0,3}>/,blockquoteSetextReplace:/\n {0,3}((?:=+|-+) *)(?=\n|$)/g,blockquoteSetextReplace2:/^ {0,3}>[ \t]?/gm,listReplaceTabs:/^\t+/,listReplaceNesting:/^ {1,4}(?=( {4})*[^ ])/g,listIsTask:/^\[[ xX]\] /,listReplaceTask:/^\[[ xX]\] +/,anyLine:/\n.*\n/,hrefBrackets:/^<(.*)>$/,tableDelimiter:/[:|]/,tableAlignChars:/^\||\| *$/g,tableRowBlankLine:/\n[ \t]*$/,tableAlignRight:/^ *-+: *$/,tableAlignCenter:/^ *:-+: *$/,tableAlignLeft:/^ *:-+ *$/,startATag:/^<a /i,endATag:/^<\/a>/i,startPreScriptTag:/^<(pre|code|kbd|script)(\s|>)/i,endPreScriptTag:/^<\/(pre|code|kbd|script)(\s|>)/i,startAngleBracket:/^</,endAngleBracket:/>$/,pedanticHrefTitle:/^([^'"]*[^\s])\s+(['"])(.*)\2/,unicodeAlphaNumeric:/[\p{L}\p{N}]/u,escapeTest:/[&<>"']/,escapeReplace:/[&<>"']/g,escapeTestNoEncode:/[<>"']|&(?!(#\d{1,7}|#[Xx][a-fA-F0-9]{1,6}|\w+);)/,escapeReplaceNoEncode:/[<>"']|&(?!(#\d{1,7}|#[Xx][a-fA-F0-9]{1,6}|\w+);)/g,unescapeTest:/&(#(?:\d+)|(?:#x[0-9A-Fa-f]+)|(?:\w+));?/ig,caret:/(^|[^\[])\^/g,percentDecode:/%25/g,findPipe:/\|/g,splitPipe:/ \|/,slashPipe:/\\\|/g,carriageReturn:/\r\n|\r/g,spaceLine:/^ +$/gm,notSpaceStart:/^\S*/,endingNewline:/\n$/,listItemRegex:i=>new RegExp(`^( {0,3}${i})((?:[	 ][^\\n]*)?(?:\\n|$))`),nextBulletRegex:i=>new RegExp(`^ {0,${Math.min(3,i-1)}}(?:[*+-]|\\d{1,9}[.)])((?:[ 	][^\\n]*)?(?:\\n|$))`),hrRegex:i=>new RegExp(`^ {0,${Math.min(3,i-1)}}((?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\\n+|$)`),fencesBeginRegex:i=>new RegExp(`^ {0,${Math.min(3,i-1)}}(?:\`\`\`|~~~)`),headingBeginRegex:i=>new RegExp(`^ {0,${Math.min(3,i-1)}}#`),htmlBeginRegex:i=>new RegExp(`^ {0,${Math.min(3,i-1)}}<(?:[a-z].*>|!--)`,"i")},WM=/^(?:[ \t]*(?:\n|$))+/,$M=/^((?: {4}| {0,3}\t)[^\n]+(?:\n(?:[ \t]*(?:\n|$))*)?)+/,XM=/^ {0,3}(`{3,}(?=[^`\n]*(?:\n|$))|~{3,})([^\n]*)(?:\n|$)(?:|([\s\S]*?)(?:\n|$))(?: {0,3}\1[~`]* *(?=\n|$)|$)/,xs=/^ {0,3}((?:-[\t ]*){3,}|(?:_[ \t]*){3,}|(?:\*[ \t]*){3,})(?:\n+|$)/,qM=/^ {0,3}(#{1,6})(?=\s|$)(.*)(?:\n+|$)/,zo=/(?:[*+-]|\d{1,9}[.)])/,tu=/^(?!bull |blockCode|fences|blockquote|heading|html|table)((?:.|\n(?!\s*?\n|bull |blockCode|fences|blockquote|heading|html|table))+?)\n {0,3}(=+|-+) *(?:\n+|$)/,nu=dt(tu).replace(/bull/g,zo).replace(/blockCode/g,/(?: {4}| {0,3}\t)/).replace(/fences/g,/ {0,3}(?:`{3,}|~{3,})/).replace(/blockquote/g,/ {0,3}>/).replace(/heading/g,/ {0,3}#{1,6}/).replace(/html/g,/ {0,3}<[^\n>]+>\n/).replace(/\|table/g,"").getRegex(),YM=dt(tu).replace(/bull/g,zo).replace(/blockCode/g,/(?: {4}| {0,3}\t)/).replace(/fences/g,/ {0,3}(?:`{3,}|~{3,})/).replace(/blockquote/g,/ {0,3}>/).replace(/heading/g,/ {0,3}#{1,6}/).replace(/html/g,/ {0,3}<[^\n>]+>\n/).replace(/table/g,/ {0,3}\|?(?:[:\- ]*\|)+[\:\- ]*\n/).getRegex(),Vo=/^([^\n]+(?:\n(?!hr|heading|lheading|blockquote|fences|list|html|table| +\n)[^\n]+)*)/,jM=/^[^\n]+/,Ho=/(?!\s*\])(?:\\.|[^\[\]\\])+/,ZM=dt(/^ {0,3}\[(label)\]: *(?:\n[ \t]*)?([^<\s][^\s]*|<.*?>)(?:(?: +(?:\n[ \t]*)?| *\n[ \t]*)(title))? *(?:\n+|$)/).replace("label",Ho).replace("title",/(?:"(?:\\"?|[^"\\])*"|'[^'\n]*(?:\n[^'\n]+)*\n?'|\([^()]*\))/).getRegex(),KM=dt(/^( {0,3}bull)([ \t][^\n]+?)?(?:\n|$)/).replace(/bull/g,zo).getRegex(),Er="address|article|aside|base|basefont|blockquote|body|caption|center|col|colgroup|dd|details|dialog|dir|div|dl|dt|fieldset|figcaption|figure|footer|form|frame|frameset|h[1-6]|head|header|hr|html|iframe|legend|li|link|main|menu|menuitem|meta|nav|noframes|ol|optgroup|option|p|param|search|section|summary|table|tbody|td|tfoot|th|thead|title|tr|track|ul",Go=/<!--(?:-?>|[\s\S]*?(?:-->|$))/,JM=dt("^ {0,3}(?:<(script|pre|style|textarea)[\\s>][\\s\\S]*?(?:</\\1>[^\\n]*\\n+|$)|comment[^\\n]*(\\n+|$)|<\\?[\\s\\S]*?(?:\\?>\\n*|$)|<![A-Z][\\s\\S]*?(?:>\\n*|$)|<!\\[CDATA\\[[\\s\\S]*?(?:\\]\\]>\\n*|$)|</?(tag)(?: +|\\n|/?>)[\\s\\S]*?(?:(?:\\n[ 	]*)+\\n|$)|<(?!script|pre|style|textarea)([a-z][\\w-]*)(?:attribute)*? */?>(?=[ \\t]*(?:\\n|$))[\\s\\S]*?(?:(?:\\n[ 	]*)+\\n|$)|</(?!script|pre|style|textarea)[a-z][\\w-]*\\s*>(?=[ \\t]*(?:\\n|$))[\\s\\S]*?(?:(?:\\n[ 	]*)+\\n|$))","i").replace("comment",Go).replace("tag",Er).replace("attribute",/ +[a-zA-Z:_][\w.:-]*(?: *= *"[^"\n]*"| *= *'[^'\n]*'| *= *[^\s"'=<>`]+)?/).getRegex(),iu=dt(Vo).replace("hr",xs).replace("heading"," {0,3}#{1,6}(?:\\s|$)").replace("|lheading","").replace("|table","").replace("blockquote"," {0,3}>").replace("fences"," {0,3}(?:`{3,}(?=[^`\\n]*\\n)|~{3,})[^\\n]*\\n").replace("list"," {0,3}(?:[*+-]|1[.)]) ").replace("html","</?(?:tag)(?: +|\\n|/?>)|<(?:script|pre|style|textarea|!--)").replace("tag",Er).getRegex(),QM=dt(/^( {0,3}> ?(paragraph|[^\n]*)(?:\n|$))+/).replace("paragraph",iu).getRegex(),Wo={blockquote:QM,code:$M,def:ZM,fences:XM,heading:qM,hr:xs,html:JM,lheading:nu,list:KM,newline:WM,paragraph:iu,table:rs,text:jM},sc=dt("^ *([^\\n ].*)\\n {0,3}((?:\\| *)?:?-+:? *(?:\\| *:?-+:? *)*(?:\\| *)?)(?:\\n((?:(?! *\\n|hr|heading|blockquote|code|fences|list|html).*(?:\\n|$))*)\\n*|$)").replace("hr",xs).replace("heading"," {0,3}#{1,6}(?:\\s|$)").replace("blockquote"," {0,3}>").replace("code","(?: {4}| {0,3}	)[^\\n]").replace("fences"," {0,3}(?:`{3,}(?=[^`\\n]*\\n)|~{3,})[^\\n]*\\n").replace("list"," {0,3}(?:[*+-]|1[.)]) ").replace("html","</?(?:tag)(?: +|\\n|/?>)|<(?:script|pre|style|textarea|!--)").replace("tag",Er).getRegex(),eS={...Wo,lheading:YM,table:sc,paragraph:dt(Vo).replace("hr",xs).replace("heading"," {0,3}#{1,6}(?:\\s|$)").replace("|lheading","").replace("table",sc).replace("blockquote"," {0,3}>").replace("fences"," {0,3}(?:`{3,}(?=[^`\\n]*\\n)|~{3,})[^\\n]*\\n").replace("list"," {0,3}(?:[*+-]|1[.)]) ").replace("html","</?(?:tag)(?: +|\\n|/?>)|<(?:script|pre|style|textarea|!--)").replace("tag",Er).getRegex()},tS={...Wo,html:dt(`^ *(?:comment *(?:\\n|\\s*$)|<(tag)[\\s\\S]+?</\\1> *(?:\\n{2,}|\\s*$)|<tag(?:"[^"]*"|'[^']*'|\\s[^'"/>\\s]*)*?/?> *(?:\\n{2,}|\\s*$))`).replace("comment",Go).replace(/tag/g,"(?!(?:a|em|strong|small|s|cite|q|dfn|abbr|data|time|code|var|samp|kbd|sub|sup|i|b|u|mark|ruby|rt|rp|bdi|bdo|span|br|wbr|ins|del|img)\\b)\\w+(?!:|[^\\w\\s@]*@)\\b").getRegex(),def:/^ *\[([^\]]+)\]: *<?([^\s>]+)>?(?: +(["(][^\n]+[")]))? *(?:\n+|$)/,heading:/^(#{1,6})(.*)(?:\n+|$)/,fences:rs,lheading:/^(.+?)\n {0,3}(=+|-+) *(?:\n+|$)/,paragraph:dt(Vo).replace("hr",xs).replace("heading",` *#{1,6} *[^
]`).replace("lheading",nu).replace("|table","").replace("blockquote"," {0,3}>").replace("|fences","").replace("|list","").replace("|html","").replace("|tag","").getRegex()},nS=/^\\([!"#$%&'()*+,\-./:;<=>?@\[\]\\^_`{|}~])/,iS=/^(`+)([^`]|[^`][\s\S]*?[^`])\1(?!`)/,su=/^( {2,}|\\)\n(?!\s*$)/,sS=/^(`+|[^`])(?:(?= {2,}\n)|[\s\S]*?(?:(?=[\\<!\[`*_]|\b_|$)|[^ ](?= {2,}\n)))/,Tr=/[\p{P}\p{S}]/u,$o=/[\s\p{P}\p{S}]/u,ru=/[^\s\p{P}\p{S}]/u,rS=dt(/^((?![*_])punctSpace)/,"u").replace(/punctSpace/g,$o).getRegex(),au=/(?!~)[\p{P}\p{S}]/u,aS=/(?!~)[\s\p{P}\p{S}]/u,oS=/(?:[^\s\p{P}\p{S}]|~)/u,lS=/\[[^[\]]*?\]\((?:\\.|[^\\\(\)]|\((?:\\.|[^\\\(\)])*\))*\)|`[^`]*?`|<(?! )[^<>]*?>/g,ou=/^(?:\*+(?:((?!\*)punct)|[^\s*]))|^_+(?:((?!_)punct)|([^\s_]))/,cS=dt(ou,"u").replace(/punct/g,Tr).getRegex(),uS=dt(ou,"u").replace(/punct/g,au).getRegex(),lu="^[^_*]*?__[^_*]*?\\*[^_*]*?(?=__)|[^*]+(?=[^*])|(?!\\*)punct(\\*+)(?=[\\s]|$)|notPunctSpace(\\*+)(?!\\*)(?=punctSpace|$)|(?!\\*)punctSpace(\\*+)(?=notPunctSpace)|[\\s](\\*+)(?!\\*)(?=punct)|(?!\\*)punct(\\*+)(?!\\*)(?=punct)|notPunctSpace(\\*+)(?=notPunctSpace)",dS=dt(lu,"gu").replace(/notPunctSpace/g,ru).replace(/punctSpace/g,$o).replace(/punct/g,Tr).getRegex(),hS=dt(lu,"gu").replace(/notPunctSpace/g,oS).replace(/punctSpace/g,aS).replace(/punct/g,au).getRegex(),fS=dt("^[^_*]*?\\*\\*[^_*]*?_[^_*]*?(?=\\*\\*)|[^_]+(?=[^_])|(?!_)punct(_+)(?=[\\s]|$)|notPunctSpace(_+)(?!_)(?=punctSpace|$)|(?!_)punctSpace(_+)(?=notPunctSpace)|[\\s](_+)(?!_)(?=punct)|(?!_)punct(_+)(?!_)(?=punct)","gu").replace(/notPunctSpace/g,ru).replace(/punctSpace/g,$o).replace(/punct/g,Tr).getRegex(),pS=dt(/\\(punct)/,"gu").replace(/punct/g,Tr).getRegex(),mS=dt(/^<(scheme:[^\s\x00-\x1f<>]*|email)>/).replace("scheme",/[a-zA-Z][a-zA-Z0-9+.-]{1,31}/).replace("email",/[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+(@)[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+(?![-_])/).getRegex(),gS=dt(Go).replace("(?:-->|$)","-->").getRegex(),_S=dt("^comment|^</[a-zA-Z][\\w:-]*\\s*>|^<[a-zA-Z][\\w-]*(?:attribute)*?\\s*/?>|^<\\?[\\s\\S]*?\\?>|^<![a-zA-Z]+\\s[\\s\\S]*?>|^<!\\[CDATA\\[[\\s\\S]*?\\]\\]>").replace("comment",gS).replace("attribute",/\s+[a-zA-Z:_][\w.:-]*(?:\s*=\s*"[^"]*"|\s*=\s*'[^']*'|\s*=\s*[^\s"'=<>`]+)?/).getRegex(),cr=/(?:\[(?:\\.|[^\[\]\\])*\]|\\.|`[^`]*`|[^\[\]\\`])*?/,vS=dt(/^!?\[(label)\]\(\s*(href)(?:(?:[ \t]*(?:\n[ \t]*)?)(title))?\s*\)/).replace("label",cr).replace("href",/<(?:\\.|[^\n<>\\])+>|[^ \t\n\x00-\x1f]*/).replace("title",/"(?:\\"?|[^"\\])*"|'(?:\\'?|[^'\\])*'|\((?:\\\)?|[^)\\])*\)/).getRegex(),cu=dt(/^!?\[(label)\]\[(ref)\]/).replace("label",cr).replace("ref",Ho).getRegex(),uu=dt(/^!?\[(ref)\](?:\[\])?/).replace("ref",Ho).getRegex(),xS=dt("reflink|nolink(?!\\()","g").replace("reflink",cu).replace("nolink",uu).getRegex(),Xo={_backpedal:rs,anyPunctuation:pS,autolink:mS,blockSkip:lS,br:su,code:iS,del:rs,emStrongLDelim:cS,emStrongRDelimAst:dS,emStrongRDelimUnd:fS,escape:nS,link:vS,nolink:uu,punctuation:rS,reflink:cu,reflinkSearch:xS,tag:_S,text:sS,url:rs},MS={...Xo,link:dt(/^!?\[(label)\]\((.*?)\)/).replace("label",cr).getRegex(),reflink:dt(/^!?\[(label)\]\s*\[([^\]]*)\]/).replace("label",cr).getRegex()},ao={...Xo,emStrongRDelimAst:hS,emStrongLDelim:uS,url:dt(/^((?:ftp|https?):\/\/|www\.)(?:[a-zA-Z0-9\-]+\.?)+[^\s<]*|^email/,"i").replace("email",/[A-Za-z0-9._+-]+(@)[a-zA-Z0-9-_]+(?:\.[a-zA-Z0-9-_]*[a-zA-Z0-9])+(?![-_])/).getRegex(),_backpedal:/(?:[^?!.,:;*_'"~()&]+|\([^)]*\)|&(?![a-zA-Z0-9]+;$)|[?!.,:;*_'"~)]+(?!$))+/,del:/^(~~?)(?=[^\s~])((?:\\.|[^\\])*?(?:\\.|[^\s~\\]))\1(?=[^~]|$)/,text:/^([`~]+|[^`~])(?:(?= {2,}\n)|(?=[a-zA-Z0-9.!#$%&'*+\/=?_`{\|}~-]+@)|[\s\S]*?(?:(?=[\\<!\[`*~_]|\b_|https?:\/\/|ftp:\/\/|www\.|$)|[^ ](?= {2,}\n)|[^a-zA-Z0-9.!#$%&'*+\/=?_`{\|}~-](?=[a-zA-Z0-9.!#$%&'*+\/=?_`{\|}~-]+@)))/},SS={...ao,br:dt(su).replace("{2,}","*").getRegex(),text:dt(ao.text).replace("\\b_","\\b_| {2,}\\n").replace(/\{2,\}/g,"*").getRegex()},qs={normal:Wo,gfm:eS,pedantic:tS},Qi={normal:Xo,gfm:ao,breaks:SS,pedantic:MS},yS={"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;","'":"&#39;"},rc=i=>yS[i];function mn(i,e){if(e){if($t.escapeTest.test(i))return i.replace($t.escapeReplace,rc)}else if($t.escapeTestNoEncode.test(i))return i.replace($t.escapeReplaceNoEncode,rc);return i}function ac(i){try{i=encodeURI(i).replace($t.percentDecode,"%")}catch{return null}return i}function oc(i,e){var r;let t=i.replace($t.findPipe,(a,o,l)=>{let c=!1,u=o;for(;--u>=0&&l[u]==="\\";)c=!c;return c?"|":" |"}),n=t.split($t.splitPipe),s=0;if(n[0].trim()||n.shift(),n.length>0&&!((r=n.at(-1))!=null&&r.trim())&&n.pop(),e)if(n.length>e)n.splice(e);else for(;n.length<e;)n.push("");for(;s<n.length;s++)n[s]=n[s].trim().replace($t.slashPipe,"|");return n}function es(i,e,t){let n=i.length;if(n===0)return"";let s=0;for(;s<n&&i.charAt(n-s-1)===e;)s++;return i.slice(0,n-s)}function bS(i,e){if(i.indexOf(e[1])===-1)return-1;let t=0;for(let n=0;n<i.length;n++)if(i[n]==="\\")n++;else if(i[n]===e[0])t++;else if(i[n]===e[1]&&(t--,t<0))return n;return t>0?-2:-1}function lc(i,e,t,n,s){let r=e.href,a=e.title||null,o=i[1].replace(s.other.outputLinkReplace,"$1");n.state.inLink=!0;let l={type:i[0].charAt(0)==="!"?"image":"link",raw:t,href:r,title:a,text:o,tokens:n.inlineTokens(o)};return n.state.inLink=!1,l}function ES(i,e,t){let n=i.match(t.other.indentCodeCompensation);if(n===null)return e;let s=n[1];return e.split(`
`).map(r=>{let a=r.match(t.other.beginningSpace);if(a===null)return r;let[o]=a;return o.length>=s.length?r.slice(s.length):r}).join(`
`)}var ur=class{constructor(i){_t(this,"options");_t(this,"rules");_t(this,"lexer");this.options=i||mi}space(i){let e=this.rules.block.newline.exec(i);if(e&&e[0].length>0)return{type:"space",raw:e[0]}}code(i){let e=this.rules.block.code.exec(i);if(e){let t=e[0].replace(this.rules.other.codeRemoveIndent,"");return{type:"code",raw:e[0],codeBlockStyle:"indented",text:this.options.pedantic?t:es(t,`
`)}}}fences(i){let e=this.rules.block.fences.exec(i);if(e){let t=e[0],n=ES(t,e[3]||"",this.rules);return{type:"code",raw:t,lang:e[2]?e[2].trim().replace(this.rules.inline.anyPunctuation,"$1"):e[2],text:n}}}heading(i){let e=this.rules.block.heading.exec(i);if(e){let t=e[2].trim();if(this.rules.other.endingHash.test(t)){let n=es(t,"#");(this.options.pedantic||!n||this.rules.other.endingSpaceChar.test(n))&&(t=n.trim())}return{type:"heading",raw:e[0],depth:e[1].length,text:t,tokens:this.lexer.inline(t)}}}hr(i){let e=this.rules.block.hr.exec(i);if(e)return{type:"hr",raw:es(e[0],`
`)}}blockquote(i){let e=this.rules.block.blockquote.exec(i);if(e){let t=es(e[0],`
`).split(`
`),n="",s="",r=[];for(;t.length>0;){let a=!1,o=[],l;for(l=0;l<t.length;l++)if(this.rules.other.blockquoteStart.test(t[l]))o.push(t[l]),a=!0;else if(!a)o.push(t[l]);else break;t=t.slice(l);let c=o.join(`
`),u=c.replace(this.rules.other.blockquoteSetextReplace,`
    $1`).replace(this.rules.other.blockquoteSetextReplace2,"");n=n?`${n}
${c}`:c,s=s?`${s}
${u}`:u;let d=this.lexer.state.top;if(this.lexer.state.top=!0,this.lexer.blockTokens(u,r,!0),this.lexer.state.top=d,t.length===0)break;let h=r.at(-1);if((h==null?void 0:h.type)==="code")break;if((h==null?void 0:h.type)==="blockquote"){let p=h,g=p.raw+`
`+t.join(`
`),_=this.blockquote(g);r[r.length-1]=_,n=n.substring(0,n.length-p.raw.length)+_.raw,s=s.substring(0,s.length-p.text.length)+_.text;break}else if((h==null?void 0:h.type)==="list"){let p=h,g=p.raw+`
`+t.join(`
`),_=this.list(g);r[r.length-1]=_,n=n.substring(0,n.length-h.raw.length)+_.raw,s=s.substring(0,s.length-p.raw.length)+_.raw,t=g.substring(r.at(-1).raw.length).split(`
`);continue}}return{type:"blockquote",raw:n,tokens:r,text:s}}}list(i){let e=this.rules.block.list.exec(i);if(e){let t=e[1].trim(),n=t.length>1,s={type:"list",raw:"",ordered:n,start:n?+t.slice(0,-1):"",loose:!1,items:[]};t=n?`\\d{1,9}\\${t.slice(-1)}`:`\\${t}`,this.options.pedantic&&(t=n?t:"[*+-]");let r=this.rules.other.listItemRegex(t),a=!1;for(;i;){let l=!1,c="",u="";if(!(e=r.exec(i))||this.rules.block.hr.test(i))break;c=e[0],i=i.substring(c.length);let d=e[2].split(`
`,1)[0].replace(this.rules.other.listReplaceTabs,f=>" ".repeat(3*f.length)),h=i.split(`
`,1)[0],p=!d.trim(),g=0;if(this.options.pedantic?(g=2,u=d.trimStart()):p?g=e[1].length+1:(g=e[2].search(this.rules.other.nonSpaceChar),g=g>4?1:g,u=d.slice(g),g+=e[1].length),p&&this.rules.other.blankLine.test(h)&&(c+=h+`
`,i=i.substring(h.length+1),l=!0),!l){let f=this.rules.other.nextBulletRegex(g),E=this.rules.other.hrRegex(g),b=this.rules.other.fencesBeginRegex(g),v=this.rules.other.headingBeginRegex(g),T=this.rules.other.htmlBeginRegex(g);for(;i;){let A=i.split(`
`,1)[0],C;if(h=A,this.options.pedantic?(h=h.replace(this.rules.other.listReplaceNesting,"  "),C=h):C=h.replace(this.rules.other.tabCharGlobal,"    "),b.test(h)||v.test(h)||T.test(h)||f.test(h)||E.test(h))break;if(C.search(this.rules.other.nonSpaceChar)>=g||!h.trim())u+=`
`+C.slice(g);else{if(p||d.replace(this.rules.other.tabCharGlobal,"    ").search(this.rules.other.nonSpaceChar)>=4||b.test(d)||v.test(d)||E.test(d))break;u+=`
`+h}!p&&!h.trim()&&(p=!0),c+=A+`
`,i=i.substring(A.length+1),d=C.slice(g)}}s.loose||(a?s.loose=!0:this.rules.other.doubleBlankLine.test(c)&&(a=!0));let _=null,m;this.options.gfm&&(_=this.rules.other.listIsTask.exec(u),_&&(m=_[0]!=="[ ] ",u=u.replace(this.rules.other.listReplaceTask,""))),s.items.push({type:"list_item",raw:c,task:!!_,checked:m,loose:!1,text:u,tokens:[]}),s.raw+=c}let o=s.items.at(-1);if(o)o.raw=o.raw.trimEnd(),o.text=o.text.trimEnd();else return;s.raw=s.raw.trimEnd();for(let l=0;l<s.items.length;l++)if(this.lexer.state.top=!1,s.items[l].tokens=this.lexer.blockTokens(s.items[l].text,[]),!s.loose){let c=s.items[l].tokens.filter(d=>d.type==="space"),u=c.length>0&&c.some(d=>this.rules.other.anyLine.test(d.raw));s.loose=u}if(s.loose)for(let l=0;l<s.items.length;l++)s.items[l].loose=!0;return s}}html(i){let e=this.rules.block.html.exec(i);if(e)return{type:"html",block:!0,raw:e[0],pre:e[1]==="pre"||e[1]==="script"||e[1]==="style",text:e[0]}}def(i){let e=this.rules.block.def.exec(i);if(e){let t=e[1].toLowerCase().replace(this.rules.other.multipleSpaceGlobal," "),n=e[2]?e[2].replace(this.rules.other.hrefBrackets,"$1").replace(this.rules.inline.anyPunctuation,"$1"):"",s=e[3]?e[3].substring(1,e[3].length-1).replace(this.rules.inline.anyPunctuation,"$1"):e[3];return{type:"def",tag:t,raw:e[0],href:n,title:s}}}table(i){var a;let e=this.rules.block.table.exec(i);if(!e||!this.rules.other.tableDelimiter.test(e[2]))return;let t=oc(e[1]),n=e[2].replace(this.rules.other.tableAlignChars,"").split("|"),s=(a=e[3])!=null&&a.trim()?e[3].replace(this.rules.other.tableRowBlankLine,"").split(`
`):[],r={type:"table",raw:e[0],header:[],align:[],rows:[]};if(t.length===n.length){for(let o of n)this.rules.other.tableAlignRight.test(o)?r.align.push("right"):this.rules.other.tableAlignCenter.test(o)?r.align.push("center"):this.rules.other.tableAlignLeft.test(o)?r.align.push("left"):r.align.push(null);for(let o=0;o<t.length;o++)r.header.push({text:t[o],tokens:this.lexer.inline(t[o]),header:!0,align:r.align[o]});for(let o of s)r.rows.push(oc(o,r.header.length).map((l,c)=>({text:l,tokens:this.lexer.inline(l),header:!1,align:r.align[c]})));return r}}lheading(i){let e=this.rules.block.lheading.exec(i);if(e)return{type:"heading",raw:e[0],depth:e[2].charAt(0)==="="?1:2,text:e[1],tokens:this.lexer.inline(e[1])}}paragraph(i){let e=this.rules.block.paragraph.exec(i);if(e){let t=e[1].charAt(e[1].length-1)===`
`?e[1].slice(0,-1):e[1];return{type:"paragraph",raw:e[0],text:t,tokens:this.lexer.inline(t)}}}text(i){let e=this.rules.block.text.exec(i);if(e)return{type:"text",raw:e[0],text:e[0],tokens:this.lexer.inline(e[0])}}escape(i){let e=this.rules.inline.escape.exec(i);if(e)return{type:"escape",raw:e[0],text:e[1]}}tag(i){let e=this.rules.inline.tag.exec(i);if(e)return!this.lexer.state.inLink&&this.rules.other.startATag.test(e[0])?this.lexer.state.inLink=!0:this.lexer.state.inLink&&this.rules.other.endATag.test(e[0])&&(this.lexer.state.inLink=!1),!this.lexer.state.inRawBlock&&this.rules.other.startPreScriptTag.test(e[0])?this.lexer.state.inRawBlock=!0:this.lexer.state.inRawBlock&&this.rules.other.endPreScriptTag.test(e[0])&&(this.lexer.state.inRawBlock=!1),{type:"html",raw:e[0],inLink:this.lexer.state.inLink,inRawBlock:this.lexer.state.inRawBlock,block:!1,text:e[0]}}link(i){let e=this.rules.inline.link.exec(i);if(e){let t=e[2].trim();if(!this.options.pedantic&&this.rules.other.startAngleBracket.test(t)){if(!this.rules.other.endAngleBracket.test(t))return;let r=es(t.slice(0,-1),"\\");if((t.length-r.length)%2===0)return}else{let r=bS(e[2],"()");if(r===-2)return;if(r>-1){let a=(e[0].indexOf("!")===0?5:4)+e[1].length+r;e[2]=e[2].substring(0,r),e[0]=e[0].substring(0,a).trim(),e[3]=""}}let n=e[2],s="";if(this.options.pedantic){let r=this.rules.other.pedanticHrefTitle.exec(n);r&&(n=r[1],s=r[3])}else s=e[3]?e[3].slice(1,-1):"";return n=n.trim(),this.rules.other.startAngleBracket.test(n)&&(this.options.pedantic&&!this.rules.other.endAngleBracket.test(t)?n=n.slice(1):n=n.slice(1,-1)),lc(e,{href:n&&n.replace(this.rules.inline.anyPunctuation,"$1"),title:s&&s.replace(this.rules.inline.anyPunctuation,"$1")},e[0],this.lexer,this.rules)}}reflink(i,e){let t;if((t=this.rules.inline.reflink.exec(i))||(t=this.rules.inline.nolink.exec(i))){let n=(t[2]||t[1]).replace(this.rules.other.multipleSpaceGlobal," "),s=e[n.toLowerCase()];if(!s){let r=t[0].charAt(0);return{type:"text",raw:r,text:r}}return lc(t,s,t[0],this.lexer,this.rules)}}emStrong(i,e,t=""){let n=this.rules.inline.emStrongLDelim.exec(i);if(!(!n||n[3]&&t.match(this.rules.other.unicodeAlphaNumeric))&&(!(n[1]||n[2])||!t||this.rules.inline.punctuation.exec(t))){let s=[...n[0]].length-1,r,a,o=s,l=0,c=n[0][0]==="*"?this.rules.inline.emStrongRDelimAst:this.rules.inline.emStrongRDelimUnd;for(c.lastIndex=0,e=e.slice(-1*i.length+s);(n=c.exec(e))!=null;){if(r=n[1]||n[2]||n[3]||n[4]||n[5]||n[6],!r)continue;if(a=[...r].length,n[3]||n[4]){o+=a;continue}else if((n[5]||n[6])&&s%3&&!((s+a)%3)){l+=a;continue}if(o-=a,o>0)continue;a=Math.min(a,a+o+l);let u=[...n[0]][0].length,d=i.slice(0,s+n.index+u+a);if(Math.min(s,a)%2){let p=d.slice(1,-1);return{type:"em",raw:d,text:p,tokens:this.lexer.inlineTokens(p)}}let h=d.slice(2,-2);return{type:"strong",raw:d,text:h,tokens:this.lexer.inlineTokens(h)}}}}codespan(i){let e=this.rules.inline.code.exec(i);if(e){let t=e[2].replace(this.rules.other.newLineCharGlobal," "),n=this.rules.other.nonSpaceChar.test(t),s=this.rules.other.startingSpaceChar.test(t)&&this.rules.other.endingSpaceChar.test(t);return n&&s&&(t=t.substring(1,t.length-1)),{type:"codespan",raw:e[0],text:t}}}br(i){let e=this.rules.inline.br.exec(i);if(e)return{type:"br",raw:e[0]}}del(i){let e=this.rules.inline.del.exec(i);if(e)return{type:"del",raw:e[0],text:e[2],tokens:this.lexer.inlineTokens(e[2])}}autolink(i){let e=this.rules.inline.autolink.exec(i);if(e){let t,n;return e[2]==="@"?(t=e[1],n="mailto:"+t):(t=e[1],n=t),{type:"link",raw:e[0],text:t,href:n,tokens:[{type:"text",raw:t,text:t}]}}}url(i){var t;let e;if(e=this.rules.inline.url.exec(i)){let n,s;if(e[2]==="@")n=e[0],s="mailto:"+n;else{let r;do r=e[0],e[0]=((t=this.rules.inline._backpedal.exec(e[0]))==null?void 0:t[0])??"";while(r!==e[0]);n=e[0],e[1]==="www."?s="http://"+e[0]:s=e[0]}return{type:"link",raw:e[0],text:n,href:s,tokens:[{type:"text",raw:n,text:n}]}}}inlineText(i){let e=this.rules.inline.text.exec(i);if(e){let t=this.lexer.state.inRawBlock;return{type:"text",raw:e[0],text:e[0],escaped:t}}}},Ln=class oo{constructor(e){_t(this,"tokens");_t(this,"options");_t(this,"state");_t(this,"tokenizer");_t(this,"inlineQueue");this.tokens=[],this.tokens.links=Object.create(null),this.options=e||mi,this.options.tokenizer=this.options.tokenizer||new ur,this.tokenizer=this.options.tokenizer,this.tokenizer.options=this.options,this.tokenizer.lexer=this,this.inlineQueue=[],this.state={inLink:!1,inRawBlock:!1,top:!0};let t={other:$t,block:qs.normal,inline:Qi.normal};this.options.pedantic?(t.block=qs.pedantic,t.inline=Qi.pedantic):this.options.gfm&&(t.block=qs.gfm,this.options.breaks?t.inline=Qi.breaks:t.inline=Qi.gfm),this.tokenizer.rules=t}static get rules(){return{block:qs,inline:Qi}}static lex(e,t){return new oo(t).lex(e)}static lexInline(e,t){return new oo(t).inlineTokens(e)}lex(e){e=e.replace($t.carriageReturn,`
`),this.blockTokens(e,this.tokens);for(let t=0;t<this.inlineQueue.length;t++){let n=this.inlineQueue[t];this.inlineTokens(n.src,n.tokens)}return this.inlineQueue=[],this.tokens}blockTokens(e,t=[],n=!1){var s,r,a;for(this.options.pedantic&&(e=e.replace($t.tabCharGlobal,"    ").replace($t.spaceLine,""));e;){let o;if((r=(s=this.options.extensions)==null?void 0:s.block)!=null&&r.some(c=>(o=c.call({lexer:this},e,t))?(e=e.substring(o.raw.length),t.push(o),!0):!1))continue;if(o=this.tokenizer.space(e)){e=e.substring(o.raw.length);let c=t.at(-1);o.raw.length===1&&c!==void 0?c.raw+=`
`:t.push(o);continue}if(o=this.tokenizer.code(e)){e=e.substring(o.raw.length);let c=t.at(-1);(c==null?void 0:c.type)==="paragraph"||(c==null?void 0:c.type)==="text"?(c.raw+=`
`+o.raw,c.text+=`
`+o.text,this.inlineQueue.at(-1).src=c.text):t.push(o);continue}if(o=this.tokenizer.fences(e)){e=e.substring(o.raw.length),t.push(o);continue}if(o=this.tokenizer.heading(e)){e=e.substring(o.raw.length),t.push(o);continue}if(o=this.tokenizer.hr(e)){e=e.substring(o.raw.length),t.push(o);continue}if(o=this.tokenizer.blockquote(e)){e=e.substring(o.raw.length),t.push(o);continue}if(o=this.tokenizer.list(e)){e=e.substring(o.raw.length),t.push(o);continue}if(o=this.tokenizer.html(e)){e=e.substring(o.raw.length),t.push(o);continue}if(o=this.tokenizer.def(e)){e=e.substring(o.raw.length);let c=t.at(-1);(c==null?void 0:c.type)==="paragraph"||(c==null?void 0:c.type)==="text"?(c.raw+=`
`+o.raw,c.text+=`
`+o.raw,this.inlineQueue.at(-1).src=c.text):this.tokens.links[o.tag]||(this.tokens.links[o.tag]={href:o.href,title:o.title});continue}if(o=this.tokenizer.table(e)){e=e.substring(o.raw.length),t.push(o);continue}if(o=this.tokenizer.lheading(e)){e=e.substring(o.raw.length),t.push(o);continue}let l=e;if((a=this.options.extensions)!=null&&a.startBlock){let c=1/0,u=e.slice(1),d;this.options.extensions.startBlock.forEach(h=>{d=h.call({lexer:this},u),typeof d=="number"&&d>=0&&(c=Math.min(c,d))}),c<1/0&&c>=0&&(l=e.substring(0,c+1))}if(this.state.top&&(o=this.tokenizer.paragraph(l))){let c=t.at(-1);n&&(c==null?void 0:c.type)==="paragraph"?(c.raw+=`
`+o.raw,c.text+=`
`+o.text,this.inlineQueue.pop(),this.inlineQueue.at(-1).src=c.text):t.push(o),n=l.length!==e.length,e=e.substring(o.raw.length);continue}if(o=this.tokenizer.text(e)){e=e.substring(o.raw.length);let c=t.at(-1);(c==null?void 0:c.type)==="text"?(c.raw+=`
`+o.raw,c.text+=`
`+o.text,this.inlineQueue.pop(),this.inlineQueue.at(-1).src=c.text):t.push(o);continue}if(e){let c="Infinite loop on byte: "+e.charCodeAt(0);if(this.options.silent){console.error(c);break}else throw new Error(c)}}return this.state.top=!0,t}inline(e,t=[]){return this.inlineQueue.push({src:e,tokens:t}),t}inlineTokens(e,t=[]){var o,l,c;let n=e,s=null;if(this.tokens.links){let u=Object.keys(this.tokens.links);if(u.length>0)for(;(s=this.tokenizer.rules.inline.reflinkSearch.exec(n))!=null;)u.includes(s[0].slice(s[0].lastIndexOf("[")+1,-1))&&(n=n.slice(0,s.index)+"["+"a".repeat(s[0].length-2)+"]"+n.slice(this.tokenizer.rules.inline.reflinkSearch.lastIndex))}for(;(s=this.tokenizer.rules.inline.anyPunctuation.exec(n))!=null;)n=n.slice(0,s.index)+"++"+n.slice(this.tokenizer.rules.inline.anyPunctuation.lastIndex);for(;(s=this.tokenizer.rules.inline.blockSkip.exec(n))!=null;)n=n.slice(0,s.index)+"["+"a".repeat(s[0].length-2)+"]"+n.slice(this.tokenizer.rules.inline.blockSkip.lastIndex);let r=!1,a="";for(;e;){r||(a=""),r=!1;let u;if((l=(o=this.options.extensions)==null?void 0:o.inline)!=null&&l.some(h=>(u=h.call({lexer:this},e,t))?(e=e.substring(u.raw.length),t.push(u),!0):!1))continue;if(u=this.tokenizer.escape(e)){e=e.substring(u.raw.length),t.push(u);continue}if(u=this.tokenizer.tag(e)){e=e.substring(u.raw.length),t.push(u);continue}if(u=this.tokenizer.link(e)){e=e.substring(u.raw.length),t.push(u);continue}if(u=this.tokenizer.reflink(e,this.tokens.links)){e=e.substring(u.raw.length);let h=t.at(-1);u.type==="text"&&(h==null?void 0:h.type)==="text"?(h.raw+=u.raw,h.text+=u.text):t.push(u);continue}if(u=this.tokenizer.emStrong(e,n,a)){e=e.substring(u.raw.length),t.push(u);continue}if(u=this.tokenizer.codespan(e)){e=e.substring(u.raw.length),t.push(u);continue}if(u=this.tokenizer.br(e)){e=e.substring(u.raw.length),t.push(u);continue}if(u=this.tokenizer.del(e)){e=e.substring(u.raw.length),t.push(u);continue}if(u=this.tokenizer.autolink(e)){e=e.substring(u.raw.length),t.push(u);continue}if(!this.state.inLink&&(u=this.tokenizer.url(e))){e=e.substring(u.raw.length),t.push(u);continue}let d=e;if((c=this.options.extensions)!=null&&c.startInline){let h=1/0,p=e.slice(1),g;this.options.extensions.startInline.forEach(_=>{g=_.call({lexer:this},p),typeof g=="number"&&g>=0&&(h=Math.min(h,g))}),h<1/0&&h>=0&&(d=e.substring(0,h+1))}if(u=this.tokenizer.inlineText(d)){e=e.substring(u.raw.length),u.raw.slice(-1)!=="_"&&(a=u.raw.slice(-1)),r=!0;let h=t.at(-1);(h==null?void 0:h.type)==="text"?(h.raw+=u.raw,h.text+=u.text):t.push(u);continue}if(e){let h="Infinite loop on byte: "+e.charCodeAt(0);if(this.options.silent){console.error(h);break}else throw new Error(h)}}return t}},dr=class{constructor(i){_t(this,"options");_t(this,"parser");this.options=i||mi}space(i){return""}code({text:i,lang:e,escaped:t}){var r;let n=(r=(e||"").match($t.notSpaceStart))==null?void 0:r[0],s=i.replace($t.endingNewline,"")+`
`;return n?'<pre><code class="language-'+mn(n)+'">'+(t?s:mn(s,!0))+`</code></pre>
`:"<pre><code>"+(t?s:mn(s,!0))+`</code></pre>
`}blockquote({tokens:i}){return`<blockquote>
${this.parser.parse(i)}</blockquote>
`}html({text:i}){return i}heading({tokens:i,depth:e}){return`<h${e}>${this.parser.parseInline(i)}</h${e}>
`}hr(i){return`<hr>
`}list(i){let e=i.ordered,t=i.start,n="";for(let a=0;a<i.items.length;a++){let o=i.items[a];n+=this.listitem(o)}let s=e?"ol":"ul",r=e&&t!==1?' start="'+t+'"':"";return"<"+s+r+`>
`+n+"</"+s+`>
`}listitem(i){var t;let e="";if(i.task){let n=this.checkbox({checked:!!i.checked});i.loose?((t=i.tokens[0])==null?void 0:t.type)==="paragraph"?(i.tokens[0].text=n+" "+i.tokens[0].text,i.tokens[0].tokens&&i.tokens[0].tokens.length>0&&i.tokens[0].tokens[0].type==="text"&&(i.tokens[0].tokens[0].text=n+" "+mn(i.tokens[0].tokens[0].text),i.tokens[0].tokens[0].escaped=!0)):i.tokens.unshift({type:"text",raw:n+" ",text:n+" ",escaped:!0}):e+=n+" "}return e+=this.parser.parse(i.tokens,!!i.loose),`<li>${e}</li>
`}checkbox({checked:i}){return"<input "+(i?'checked="" ':"")+'disabled="" type="checkbox">'}paragraph({tokens:i}){return`<p>${this.parser.parseInline(i)}</p>
`}table(i){let e="",t="";for(let s=0;s<i.header.length;s++)t+=this.tablecell(i.header[s]);e+=this.tablerow({text:t});let n="";for(let s=0;s<i.rows.length;s++){let r=i.rows[s];t="";for(let a=0;a<r.length;a++)t+=this.tablecell(r[a]);n+=this.tablerow({text:t})}return n&&(n=`<tbody>${n}</tbody>`),`<table>
<thead>
`+e+`</thead>
`+n+`</table>
`}tablerow({text:i}){return`<tr>
${i}</tr>
`}tablecell(i){let e=this.parser.parseInline(i.tokens),t=i.header?"th":"td";return(i.align?`<${t} align="${i.align}">`:`<${t}>`)+e+`</${t}>
`}strong({tokens:i}){return`<strong>${this.parser.parseInline(i)}</strong>`}em({tokens:i}){return`<em>${this.parser.parseInline(i)}</em>`}codespan({text:i}){return`<code>${mn(i,!0)}</code>`}br(i){return"<br>"}del({tokens:i}){return`<del>${this.parser.parseInline(i)}</del>`}link({href:i,title:e,tokens:t}){let n=this.parser.parseInline(t),s=ac(i);if(s===null)return n;i=s;let r='<a href="'+i+'"';return e&&(r+=' title="'+mn(e)+'"'),r+=">"+n+"</a>",r}image({href:i,title:e,text:t,tokens:n}){n&&(t=this.parser.parseInline(n,this.parser.textRenderer));let s=ac(i);if(s===null)return mn(t);i=s;let r=`<img src="${i}" alt="${t}"`;return e&&(r+=` title="${mn(e)}"`),r+=">",r}text(i){return"tokens"in i&&i.tokens?this.parser.parseInline(i.tokens):"escaped"in i&&i.escaped?i.text:mn(i.text)}},qo=class{strong({text:i}){return i}em({text:i}){return i}codespan({text:i}){return i}del({text:i}){return i}html({text:i}){return i}text({text:i}){return i}link({text:i}){return""+i}image({text:i}){return""+i}br(){return""}},Dn=class lo{constructor(e){_t(this,"options");_t(this,"renderer");_t(this,"textRenderer");this.options=e||mi,this.options.renderer=this.options.renderer||new dr,this.renderer=this.options.renderer,this.renderer.options=this.options,this.renderer.parser=this,this.textRenderer=new qo}static parse(e,t){return new lo(t).parse(e)}static parseInline(e,t){return new lo(t).parseInline(e)}parse(e,t=!0){var s,r;let n="";for(let a=0;a<e.length;a++){let o=e[a];if((r=(s=this.options.extensions)==null?void 0:s.renderers)!=null&&r[o.type]){let c=o,u=this.options.extensions.renderers[c.type].call({parser:this},c);if(u!==!1||!["space","hr","heading","code","table","blockquote","list","html","paragraph","text"].includes(c.type)){n+=u||"";continue}}let l=o;switch(l.type){case"space":{n+=this.renderer.space(l);continue}case"hr":{n+=this.renderer.hr(l);continue}case"heading":{n+=this.renderer.heading(l);continue}case"code":{n+=this.renderer.code(l);continue}case"table":{n+=this.renderer.table(l);continue}case"blockquote":{n+=this.renderer.blockquote(l);continue}case"list":{n+=this.renderer.list(l);continue}case"html":{n+=this.renderer.html(l);continue}case"paragraph":{n+=this.renderer.paragraph(l);continue}case"text":{let c=l,u=this.renderer.text(c);for(;a+1<e.length&&e[a+1].type==="text";)c=e[++a],u+=`
`+this.renderer.text(c);t?n+=this.renderer.paragraph({type:"paragraph",raw:u,text:u,tokens:[{type:"text",raw:u,text:u,escaped:!0}]}):n+=u;continue}default:{let c='Token with "'+l.type+'" type was not found.';if(this.options.silent)return console.error(c),"";throw new Error(c)}}}return n}parseInline(e,t=this.renderer){var s,r;let n="";for(let a=0;a<e.length;a++){let o=e[a];if((r=(s=this.options.extensions)==null?void 0:s.renderers)!=null&&r[o.type]){let c=this.options.extensions.renderers[o.type].call({parser:this},o);if(c!==!1||!["escape","html","link","image","strong","em","codespan","br","del","text"].includes(o.type)){n+=c||"";continue}}let l=o;switch(l.type){case"escape":{n+=t.text(l);break}case"html":{n+=t.html(l);break}case"link":{n+=t.link(l);break}case"image":{n+=t.image(l);break}case"strong":{n+=t.strong(l);break}case"em":{n+=t.em(l);break}case"codespan":{n+=t.codespan(l);break}case"br":{n+=t.br(l);break}case"del":{n+=t.del(l);break}case"text":{n+=t.text(l);break}default:{let c='Token with "'+l.type+'" type was not found.';if(this.options.silent)return console.error(c),"";throw new Error(c)}}}return n}},ua,nr=(ua=class{constructor(i){_t(this,"options");_t(this,"block");this.options=i||mi}preprocess(i){return i}postprocess(i){return i}processAllTokens(i){return i}provideLexer(){return this.block?Ln.lex:Ln.lexInline}provideParser(){return this.block?Dn.parse:Dn.parseInline}},_t(ua,"passThroughHooks",new Set(["preprocess","postprocess","processAllTokens"])),ua),TS=class{constructor(...i){_t(this,"defaults",Bo());_t(this,"options",this.setOptions);_t(this,"parse",this.parseMarkdown(!0));_t(this,"parseInline",this.parseMarkdown(!1));_t(this,"Parser",Dn);_t(this,"Renderer",dr);_t(this,"TextRenderer",qo);_t(this,"Lexer",Ln);_t(this,"Tokenizer",ur);_t(this,"Hooks",nr);this.use(...i)}walkTokens(i,e){var n,s;let t=[];for(let r of i)switch(t=t.concat(e.call(this,r)),r.type){case"table":{let a=r;for(let o of a.header)t=t.concat(this.walkTokens(o.tokens,e));for(let o of a.rows)for(let l of o)t=t.concat(this.walkTokens(l.tokens,e));break}case"list":{let a=r;t=t.concat(this.walkTokens(a.items,e));break}default:{let a=r;(s=(n=this.defaults.extensions)==null?void 0:n.childTokens)!=null&&s[a.type]?this.defaults.extensions.childTokens[a.type].forEach(o=>{let l=a[o].flat(1/0);t=t.concat(this.walkTokens(l,e))}):a.tokens&&(t=t.concat(this.walkTokens(a.tokens,e)))}}return t}use(...i){let e=this.defaults.extensions||{renderers:{},childTokens:{}};return i.forEach(t=>{let n={...t};if(n.async=this.defaults.async||n.async||!1,t.extensions&&(t.extensions.forEach(s=>{if(!s.name)throw new Error("extension name required");if("renderer"in s){let r=e.renderers[s.name];r?e.renderers[s.name]=function(...a){let o=s.renderer.apply(this,a);return o===!1&&(o=r.apply(this,a)),o}:e.renderers[s.name]=s.renderer}if("tokenizer"in s){if(!s.level||s.level!=="block"&&s.level!=="inline")throw new Error("extension level must be 'block' or 'inline'");let r=e[s.level];r?r.unshift(s.tokenizer):e[s.level]=[s.tokenizer],s.start&&(s.level==="block"?e.startBlock?e.startBlock.push(s.start):e.startBlock=[s.start]:s.level==="inline"&&(e.startInline?e.startInline.push(s.start):e.startInline=[s.start]))}"childTokens"in s&&s.childTokens&&(e.childTokens[s.name]=s.childTokens)}),n.extensions=e),t.renderer){let s=this.defaults.renderer||new dr(this.defaults);for(let r in t.renderer){if(!(r in s))throw new Error(`renderer '${r}' does not exist`);if(["options","parser"].includes(r))continue;let a=r,o=t.renderer[a],l=s[a];s[a]=(...c)=>{let u=o.apply(s,c);return u===!1&&(u=l.apply(s,c)),u||""}}n.renderer=s}if(t.tokenizer){let s=this.defaults.tokenizer||new ur(this.defaults);for(let r in t.tokenizer){if(!(r in s))throw new Error(`tokenizer '${r}' does not exist`);if(["options","rules","lexer"].includes(r))continue;let a=r,o=t.tokenizer[a],l=s[a];s[a]=(...c)=>{let u=o.apply(s,c);return u===!1&&(u=l.apply(s,c)),u}}n.tokenizer=s}if(t.hooks){let s=this.defaults.hooks||new nr;for(let r in t.hooks){if(!(r in s))throw new Error(`hook '${r}' does not exist`);if(["options","block"].includes(r))continue;let a=r,o=t.hooks[a],l=s[a];nr.passThroughHooks.has(r)?s[a]=c=>{if(this.defaults.async)return Promise.resolve(o.call(s,c)).then(d=>l.call(s,d));let u=o.call(s,c);return l.call(s,u)}:s[a]=(...c)=>{let u=o.apply(s,c);return u===!1&&(u=l.apply(s,c)),u}}n.hooks=s}if(t.walkTokens){let s=this.defaults.walkTokens,r=t.walkTokens;n.walkTokens=function(a){let o=[];return o.push(r.call(this,a)),s&&(o=o.concat(s.call(this,a))),o}}this.defaults={...this.defaults,...n}}),this}setOptions(i){return this.defaults={...this.defaults,...i},this}lexer(i,e){return Ln.lex(i,e??this.defaults)}parser(i,e){return Dn.parse(i,e??this.defaults)}parseMarkdown(i){return(e,t)=>{let n={...t},s={...this.defaults,...n},r=this.onError(!!s.silent,!!s.async);if(this.defaults.async===!0&&n.async===!1)return r(new Error("marked(): The async option was set to true by an extension. Remove async: false from the parse options object to return a Promise."));if(typeof e>"u"||e===null)return r(new Error("marked(): input parameter is undefined or null"));if(typeof e!="string")return r(new Error("marked(): input parameter is of type "+Object.prototype.toString.call(e)+", string expected"));s.hooks&&(s.hooks.options=s,s.hooks.block=i);let a=s.hooks?s.hooks.provideLexer():i?Ln.lex:Ln.lexInline,o=s.hooks?s.hooks.provideParser():i?Dn.parse:Dn.parseInline;if(s.async)return Promise.resolve(s.hooks?s.hooks.preprocess(e):e).then(l=>a(l,s)).then(l=>s.hooks?s.hooks.processAllTokens(l):l).then(l=>s.walkTokens?Promise.all(this.walkTokens(l,s.walkTokens)).then(()=>l):l).then(l=>o(l,s)).then(l=>s.hooks?s.hooks.postprocess(l):l).catch(r);try{s.hooks&&(e=s.hooks.preprocess(e));let l=a(e,s);s.hooks&&(l=s.hooks.processAllTokens(l)),s.walkTokens&&this.walkTokens(l,s.walkTokens);let c=o(l,s);return s.hooks&&(c=s.hooks.postprocess(c)),c}catch(l){return r(l)}}}onError(i,e){return t=>{if(t.message+=`
Please report this to https://github.com/markedjs/marked.`,i){let n="<p>An error occurred:</p><pre>"+mn(t.message+"",!0)+"</pre>";return e?Promise.resolve(n):n}if(e)return Promise.reject(t);throw t}}},hi=new TS;function ht(i,e){return hi.parse(i,e)}ht.options=ht.setOptions=function(i){return hi.setOptions(i),ht.defaults=hi.defaults,eu(ht.defaults),ht};ht.getDefaults=Bo;ht.defaults=mi;ht.use=function(...i){return hi.use(...i),ht.defaults=hi.defaults,eu(ht.defaults),ht};ht.walkTokens=function(i,e){return hi.walkTokens(i,e)};ht.parseInline=hi.parseInline;ht.Parser=Dn;ht.parser=Dn.parse;ht.Renderer=dr;ht.TextRenderer=qo;ht.Lexer=Ln;ht.lexer=Ln.lex;ht.Tokenizer=ur;ht.Hooks=nr;ht.parse=ht;ht.options;ht.setOptions;ht.use;ht.walkTokens;ht.parseInline;Dn.parse;Ln.lex;const wS={class:"changelog-container"},AS={class:"version-title"},RS=["innerHTML"],CS={class:"view-more"},PS=["href"],LS={__name:"LatestRelease",setup(i){const e=We([{version:"0.10.1",content:"",renderedContent:""},{version:"0.10.0",content:"",renderedContent:""},{version:"0.9.0",content:"",renderedContent:""}]);return zt(async()=>{try{const t=await fetch(Oi("/changelogs/alpha.md"));if(t.ok){const s=(await t.text()).match(/## v[\d.]+[\s\S]*?(?=## v[\d.]+|$)/g);if(s&&s.length>0){const r=Math.min(3,s.length);for(let a=0;a<r;a++){const o=s[a],l=o.match(/## v([\d.]+)/);if(l&&l[1]){const c=l[1];let u=o.replace(/## v[\d.]+\s*/,"");u.length>200&&(u=u.substring(0,200)+"...");const d=ht.parse(u);e.value[a]={version:c,content:u,renderedContent:d}}}}}}catch(t){console.error("Failed to load changelog information:",t)}}),(t,n)=>(L(),F("div",wS,[n[0]||(n[0]=w("h3",{class:"changelog-title"},"Latest Releases",-1)),(L(!0),F(tt,null,vt(e.value,(s,r)=>(L(),F("div",{key:r,class:"version-block"},[w("h4",AS,"v"+xe(s.version),1),w("div",{class:"version-content",innerHTML:s.renderedContent},null,8,RS)]))),128)),w("p",CS,[w("a",{href:V(Oi)("/changelogs/alpha.html")},"View Full Changelog",8,PS)])]))}},DS=be(LS,[["__scopeId","data-v-10aea385"]]),IS={name:"BrowserCompatData",data(){return{compatData:[],filteredAPIs:[],groupedAPIs:{},browserInfo:null,versions:[],loading:!0,error:!1,searchQuery:"",selectedCategory:"all",selectedVersion:"all",totalAPIs:0,stableAPIs:0,experimentalAPIs:0,deprecatedAPIs:0}},mounted(){this.initializeData()},methods:{withBase:Oi,async initializeData(){this.loading=!0,this.error=!1;try{await this.loadRuntimeVersions(),await this.loadCompatibilityData(),this.setupThemeToggle(),this.updateStats(),this.filterAPIs(),this.$nextTick(()=>{this.renderVersionChart()}),this.loading=!1}catch(i){console.error("Failed to initialize compatibility data:",i),this.loading=!1,this.error=!0}},async loadRuntimeVersions(){try{const i=await fetch("https://api.github.com/repos/M-CreativeLab/jsar-runtime/releases");if(i.ok){const e=await i.json();this.versions=e.filter(t=>!t.draft&&!t.prerelease).map(t=>t.tag_name.replace(/^v/,"")).sort(this.compareVersions).reverse();return}}catch(i){console.warn("Failed to load versions from GitHub API:",i)}try{const i=await fetch("/package.json");if(i.ok){const e=await i.json();this.versions=[e.version];return}}catch(i){console.warn("Failed to load version from package.json:",i)}this.versions=["0.1.0","0.2.0","0.3.0"]},async loadCompatibilityData(){try{const i=await this.loadJSON(this.withBase("/api/browser-compat-data/all.json"));this.browserInfo=i.browsers.jsar;const e=[];i.api&&Object.entries(i.api).forEach(([t,n])=>{this.processAPIData(e,t,n,"api")}),i.html&&i.html.elements&&Object.entries(i.html.elements).forEach(([t,n])=>{this.processAPIData(e,t,n,"html")}),i.html&&i.html.global_attributes&&Object.entries(i.html.global_attributes).forEach(([t,n])=>{this.processAPIData(e,t,n,"attribute")}),i.css&&i.css.properties&&Object.entries(i.css.properties).forEach(([t,n])=>{this.processAPIData(e,t,n,"css")}),this.compatData=e}catch(i){throw console.error("Error loading compatibility data:",i),i}},async loadJSON(i){const e=await fetch(i);if(!e.ok)throw new Error(`Failed to load ${i}: ${e.status} ${e.statusText}`);return await e.json()},processAPIData(i,e,t,n){var s;if(t.__compat){const r=t.__compat,a=(s=r.support)==null?void 0:s.jsar;a&&i.push(this.createAPIItem(e,r,a,n))}Object.entries(t).forEach(([r,a])=>{r!=="__compat"&&typeof a=="object"&&this.processAPIData(i,`${e}.${r}`,a,n)})},createAPIItem(i,e,t,n){var l,c,u,d;const s=t.version_added||"unknown",r=((l=t.flags)==null?void 0:l.length)>0||((c=e.status)==null?void 0:c.experimental)===!0,a=((u=e.status)==null?void 0:u.deprecated)===!0,o=((d=e.status)==null?void 0:d.standard_track)!==!1;return{id:`${n}-${i}`,name:i,version:s,experimental:r,deprecated:a,standardTrack:o,category:n,parent:i.includes(".")?i.split(".")[0]:n,mdnUrl:e.mdn_url,specUrl:e.spec_url}},setupThemeToggle(){},filterAPIs(){const i=this.searchQuery.toLowerCase().trim();this.filteredAPIs=this.compatData.filter(e=>{const t=i===""||e.name.toLowerCase().includes(i),n=this.selectedCategory==="all"||e.category===this.selectedCategory,s=this.selectedVersion==="all"||e.version!=="unknown"&&this.compareVersions(e.version,this.selectedVersion)<=0;return t&&n&&s}),this.groupedAPIs=this.groupAPIsByParent(this.filteredAPIs)},compareVersions(i,e){const t=i.split(".").map(Number),n=e.split(".").map(Number);for(let s=0;s<Math.max(t.length,n.length);s++){const r=t[s]||0,a=n[s]||0;if(r!==a)return r-a}return 0},updateStats(){this.totalAPIs=this.compatData.length,this.stableAPIs=this.compatData.filter(i=>i.standardTrack&&!i.experimental&&!i.deprecated).length,this.experimentalAPIs=this.compatData.filter(i=>i.experimental&&!i.deprecated).length,this.deprecatedAPIs=this.compatData.filter(i=>i.deprecated).length},groupAPIsByParent(i){const e={};return i.forEach(t=>{const n=t.parent;e[n]||(e[n]=[]),e[n].push(t)}),Object.fromEntries(Object.entries(e).sort((t,n)=>t[0].localeCompare(n[0])))},getCategoryColor(i){switch(i){case"api":return"border-accent";case"html":return"border-success";case"global":return"border-warning";default:return"border-gray-500"}},getCategoryIcon(i){switch(i){case"api":return"⚙️";case"html":return"🏗️";case"global":return"🌐";default:return"📁"}},renderVersionChart(){var d;if(!this.browserInfo||!((d=this.browserInfo.jsar)!=null&&d.releases)){console.warn("Browser info not available for chart rendering");return}const i=document.getElementById("version-chart");if(!i){console.warn("Chart container not found");return}const t=Object.keys(this.browserInfo.jsar.releases).sort(this.compareVersions).map(h=>{const p=this.compatData.filter(_=>_.version<=h&&_.standardTrack&&!_.experimental&&!_.deprecated).length,g=this.compatData.filter(_=>_.version<=h&&_.experimental&&!_.deprecated).length;return{version:h,stable:p,experimental:g,total:p+g}}),n=Math.max(...t.map(h=>h.total)),s=i.offsetWidth||800,r=250,a={top:20,right:40,bottom:60,left:40},o=s-a.left-a.right,l=r-a.top-a.bottom,c=`
        <div class="chart-legend">
          <div class="legend-item">
            <div class="legend-color" style="background-color: #10b981;"></div>
            <span>Stable APIs</span>
          </div>
          <div class="legend-item">
            <div class="legend-color" style="background-color: #f59e0b;"></div>
            <span>Experimental APIs</span>
          </div>
        </div>
        <div class="chart-container" style="height: ${r}px; position: relative;">
          <svg width="${s}" height="${r}" style="display: block;">
            <!-- Grid lines -->
            ${Array.from({length:6},(h,p)=>{const g=a.top+l/5*p,_=Math.round(n-n/5*p);return`
                <line x1="${a.left}" y1="${g}" x2="${s-a.right}" y2="${g}" 
                      stroke="var(--border-color)" stroke-width="1" opacity="0.3"/>
                <text x="${a.left-10}" y="${g+4}" fill="var(--text-tertiary)" 
                      font-size="12" text-anchor="end">${_}</text>
              `}).join("")}
            
            <!-- X-axis labels -->
            ${t.map((h,p)=>`
                <text x="${a.left+o/(t.length-1)*p}" y="${r-10}" fill="var(--text-tertiary)" 
                      font-size="11" text-anchor="middle">v${h.version}</text>
              `).join("")}
            
            <!-- Stable APIs line -->
            <polyline
              fill="none"
              stroke="#10b981"
              stroke-width="3"
              points="${t.map((h,p)=>{const g=a.left+o/(t.length-1)*p,_=a.top+l-h.stable/n*l;return`${g},${_}`}).join(" ")}"
            />
            
            <!-- Experimental APIs line -->
            <polyline
              fill="none"
              stroke="#f59e0b"
              stroke-width="3"
              points="${t.map((h,p)=>{const g=a.left+o/(t.length-1)*p,_=a.top+l-h.experimental/n*l;return`${g},${_}`}).join(" ")}"
            />
            
            <!-- Data points for stable APIs -->
            ${t.map((h,p)=>{const g=a.left+o/(t.length-1)*p,_=a.top+l-h.stable/n*l;return`
                <circle cx="${g}" cy="${_}" r="4" fill="#10b981" stroke="#ffffff" stroke-width="2"
                        class="chart-point" data-tooltip="v${h.version}: ${h.stable} stable APIs" 
                        style="cursor: pointer;"/>
              `}).join("")}
            
            <!-- Data points for experimental APIs -->
            ${t.map((h,p)=>{const g=a.left+o/(t.length-1)*p,_=a.top+l-h.experimental/n*l;return`
                <circle cx="${g}" cy="${_}" r="4" fill="#f59e0b" stroke="#ffffff" stroke-width="2"
                        class="chart-point" data-tooltip="v${h.version}: ${h.experimental} experimental APIs" 
                        style="cursor: pointer;"/>
              `}).join("")}
          </svg>
        </div>
      `;i.innerHTML=c;const u=document.createElement("div");u.className="chart-tooltip",i.appendChild(u),i.querySelectorAll(".chart-point").forEach(h=>{h.addEventListener("mouseenter",p=>{u.innerHTML=p.target.getAttribute("data-tooltip"),u.style.display="block"}),h.addEventListener("mousemove",p=>{const g=i.getBoundingClientRect();u.style.left=p.clientX-g.left+10+"px",u.style.top=p.clientY-g.top-10+"px"}),h.addEventListener("mouseleave",()=>{u.style.display="none"})})},resetFilters(){this.searchQuery="",this.selectedCategory="all",this.selectedVersion="all",this.filterAPIs()},escapeHtml(i){const e=document.createElement("div");return e.textContent=i,e.innerHTML}}},US={class:"browser-compat-container"},kS={class:"compat-card"},NS={class:"search-filter-container"},FS={class:"search-box"},OS={class:"filter-container"},BS={class:"filter-group"},zS={class:"filter-group"},VS=["value"],HS={class:"stats-container"},GS={class:"stat-item"},WS={class:"stat-value"},$S={class:"stat-item"},XS={class:"stat-value"},qS={class:"stat-item"},YS={class:"stat-value"},jS={class:"stat-item"},ZS={class:"stat-value"},KS={key:0,class:"loading-container"},JS={key:1,class:"error-container"},QS={key:2,class:"empty-state"},ey={key:3,class:"api-list"},ty={class:"group-icon"},ny={class:"group-name"},iy={class:"group-count"},sy={class:"api-items"},ry={class:"api-item-row"},ay={class:"api-main-info"},oy={class:"api-name-section"},ly={class:"api-name"},cy={class:"api-badges"},uy={key:0,class:"badge bg-red-100 text-error"},dy={key:1,class:"badge bg-yellow-100 text-warning"},hy={key:2,class:"badge bg-green-100 text-success"},fy={class:"version-badge"},py={class:"api-links"},my=["href"],gy=["href"];function _y(i,e,t,n,s,r){return L(),F("div",US,[e[25]||(e[25]=w("header",{class:"compat-header"},[w("h1",null,"JSAR Browser Compatibility Data"),w("p",{class:"description"},"Explore the Web APIs supported in JSAR Runtime across different versions")],-1)),w("div",kS,[w("div",NS,[w("div",FS,[Ui(w("input",{type:"text","onUpdate:modelValue":e[0]||(e[0]=a=>s.searchQuery=a),placeholder:"Search APIs...",onInput:e[1]||(e[1]=(...a)=>r.filterAPIs&&r.filterAPIs(...a)),class:"search-input"},null,544),[[fo,s.searchQuery]]),s.searchQuery?(L(),F("button",{key:0,onClick:e[2]||(e[2]=a=>{s.searchQuery="",r.filterAPIs()}),class:"clear-button"}," ✕ ")):_e("",!0)]),w("div",OS,[w("div",BS,[e[10]||(e[10]=w("label",{for:"category-filter"},"Category:",-1)),Ui(w("select",{id:"category-filter","onUpdate:modelValue":e[3]||(e[3]=a=>s.selectedCategory=a),onChange:e[4]||(e[4]=(...a)=>r.filterAPIs&&r.filterAPIs(...a)),class:"filter-select"},e[9]||(e[9]=[w("option",{value:"all"},"All Categories",-1),w("option",{value:"api"},"Web APIs",-1),w("option",{value:"html"},"HTML Elements",-1),w("option",{value:"global"},"Global Attributes",-1)]),544),[[pa,s.selectedCategory]])]),w("div",zS,[e[12]||(e[12]=w("label",{for:"version-filter"},"Version:",-1)),Ui(w("select",{id:"version-filter","onUpdate:modelValue":e[5]||(e[5]=a=>s.selectedVersion=a),onChange:e[6]||(e[6]=(...a)=>r.filterAPIs&&r.filterAPIs(...a)),class:"filter-select"},[e[11]||(e[11]=w("option",{value:"all"},"All Versions",-1)),(L(!0),F(tt,null,vt(s.versions,a=>(L(),F("option",{key:a,value:a},xe(a),9,VS))),128))],544),[[pa,s.selectedVersion]])])])]),w("div",HS,[w("div",GS,[w("div",WS,xe(s.totalAPIs),1),e[13]||(e[13]=w("div",{class:"stat-label"},"Total APIs",-1))]),w("div",$S,[w("div",XS,xe(s.stableAPIs),1),e[14]||(e[14]=w("div",{class:"stat-label"},"Stable",-1))]),w("div",qS,[w("div",YS,xe(s.experimentalAPIs),1),e[15]||(e[15]=w("div",{class:"stat-label"},"Experimental",-1))]),w("div",jS,[w("div",ZS,xe(s.deprecatedAPIs),1),e[16]||(e[16]=w("div",{class:"stat-label"},"Deprecated",-1))])]),e[24]||(e[24]=w("div",{id:"version-chart",class:"version-chart"},null,-1)),s.loading?(L(),F("div",KS,e[17]||(e[17]=[w("div",{class:"loading-spinner"},null,-1),w("p",null,"Loading compatibility data...",-1)]))):s.error?(L(),F("div",JS,[e[18]||(e[18]=w("div",{class:"error-icon"},"⚠️",-1)),e[19]||(e[19]=w("h3",null,"Failed to load compatibility data",-1)),e[20]||(e[20]=w("p",null,"Unable to fetch API compatibility information",-1)),w("button",{onClick:e[7]||(e[7]=(...a)=>r.initializeData&&r.initializeData(...a)),class:"retry-button"},"🔄 Retry")])):s.filteredAPIs.length===0?(L(),F("div",QS,[e[21]||(e[21]=w("div",{class:"empty-icon"},"🔍",-1)),e[22]||(e[22]=w("h3",null,"No APIs found",-1)),e[23]||(e[23]=w("p",null,"Try adjusting your search or filters",-1)),w("button",{onClick:e[8]||(e[8]=(...a)=>r.resetFilters&&r.resetFilters(...a)),class:"reset-button"},"Reset Filters")])):(L(),F("div",ey,[(L(!0),F(tt,null,vt(s.groupedAPIs,(a,o)=>(L(),F("div",{key:o,class:"api-group"},[w("div",{class:ct(["api-group-header",r.getCategoryColor(a[0].category)])},[w("span",ty,xe(r.getCategoryIcon(a[0].category)),1),w("h3",ny,xe(o),1),w("span",iy,xe(a.length),1)],2),w("div",sy,[(L(!0),F(tt,null,vt(a,l=>(L(),F("div",{key:l.id,class:"api-item"},[w("div",ry,[w("div",ay,[w("div",oy,[w("h4",ly,xe(l.name),1),w("div",cy,[l.deprecated?(L(),F("span",uy,"⚠️ Deprecated")):l.experimental?(L(),F("span",dy,"🧪 Experimental")):l.standardTrack?(L(),F("span",hy,"✅ Stable")):_e("",!0),w("span",fy,xe(l.version),1)])]),w("div",py,[l.mdnUrl?(L(),F("a",{key:0,href:l.mdnUrl,target:"_blank",class:"doc-link mdn-link",title:"MDN Documentation"},"📚 MDN",8,my)):_e("",!0),l.specUrl?(L(),F("a",{key:1,href:l.specUrl,target:"_blank",class:"doc-link spec-link",title:"Official Specification"},"📋 Spec",8,gy)):_e("",!0)])])])]))),128))])]))),128))]))])])}const vy=be(IS,[["render",_y],["__scopeId","data-v-6230c33a"]]),xy={class:"download-page"},My={class:"hero-section"},Sy={class:"hero-content"},yy={class:"carousel-container"},by=["src","alt"],Ey={class:"carousel-indicators"},Ty=["onClick","aria-label"],wy={class:"downloads-container"},Ay={class:"downloads-wrapper"},Ry={class:"section-header"},Cy={class:"section-title"},Py={class:"section-subtitle"},Ly={class:"platforms-grid"},Dy={class:"platform-icon"},Iy={class:"platform-info"},Uy={class:"platform-name"},ky={class:"platform-requirements"},Ny={class:"download-button-wrapper"},Fy=["href"],Oy=["title"],By={__name:"DownloadPage",setup(i){const e=We(0),t=We([{src:"https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/docs/macos-screenshot-00.png",alt:"Mixed 2D and 3D Elements"},{src:"https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/docs/macos-screenshot-01.png",alt:"2D Elements"},{src:"https://raw.githubusercontent.com/M-CreativeLab/jsar-runtime/refs/heads/main/docs/macos-screenshot-02.png",alt:"3D Particles"}]),n=()=>{e.value=(e.value+1)%t.value.length},s=()=>{e.value=e.value===0?t.value.length-1:e.value-1},r=u=>{e.value=u};let a=null;const o=()=>{a=setInterval(n,5e3)},l=()=>{a&&(clearInterval(a),a=null)};zt(()=>{o()});const c=We([{title:"Transmute Browser",subtitle:"Try JSAR browser engine on your desktop.",platforms:[{name:"macOS",url:"https://github.com/M-CreativeLab/jsar-runtime/releases/latest/download/TransmuteBrowser.zip",requirements:"macOS 14 or later"}]}]);return(u,d)=>(L(),F("div",xy,[w("section",My,[w("div",Sy,[d[2]||(d[2]=w("h1",{class:"hero-title"},"JSAR Downloads",-1)),d[3]||(d[3]=w("p",{class:"hero-subtitle"}," Experience spatial Web browsing with JSAR. ",-1)),w("div",{class:"hero-carousel",onMouseenter:l,onMouseleave:o},[w("div",yy,[w("div",{class:"carousel-slides",style:ho({transform:`translateX(-${e.value*100}%)`})},[(L(!0),F(tt,null,vt(t.value,(h,p)=>(L(),F("div",{key:p,class:"carousel-slide"},[w("img",{src:h.src,alt:h.alt,class:"screenshot-image"},null,8,by)]))),128))],4),w("button",{class:"carousel-btn carousel-btn-prev",onClick:s,"aria-label":"Previous"},d[0]||(d[0]=[w("svg",{width:"24",height:"24",viewBox:"0 0 24 24",fill:"none"},[w("path",{d:"M15 18L9 12L15 6",stroke:"currentColor","stroke-width":"2","stroke-linecap":"round","stroke-linejoin":"round"})],-1)])),w("button",{class:"carousel-btn carousel-btn-next",onClick:n,"aria-label":"Next"},d[1]||(d[1]=[w("svg",{width:"24",height:"24",viewBox:"0 0 24 24",fill:"none"},[w("path",{d:"M9 18L15 12L9 6",stroke:"currentColor","stroke-width":"2","stroke-linecap":"round","stroke-linejoin":"round"})],-1)]))]),w("div",Ey,[(L(!0),F(tt,null,vt(t.value,(h,p)=>(L(),F("button",{key:p,class:ct(["carousel-indicator",{active:e.value===p}]),onClick:g=>r(p),"aria-label":`Go to slide ${p+1}`},null,10,Ty))),128))])],32)])]),w("section",wy,[w("div",Ay,[(L(!0),F(tt,null,vt(c.value,(h,p)=>(L(),F("div",{key:p,class:"download-section"},[w("div",Ry,[w("h2",Cy,xe(h.title),1),w("p",Py,xe(h.subtitle),1)]),w("div",Ly,[(L(!0),F(tt,null,vt(h.platforms,g=>(L(),F("div",{key:g.name,class:"platform-card"},[w("div",Dy,xe(g.icon),1),w("div",Iy,[w("h3",Uy,xe(g.name),1),w("p",ky,xe(g.requirements),1)]),w("div",Ny,[g.url?(L(),F("a",{key:0,href:g.url,class:"download-button",target:"_blank",rel:"noopener noreferrer"}," Download ",8,Fy)):(L(),F("button",{key:1,class:"download-button download-button-disabled",disabled:"",title:"Download not available for "+g.name}," Download ",8,Oy))])]))),128))])]))),128))])]),d[4]||(d[4]=mc('<section class="help-section" data-v-cd2ec5b6><div class="help-container" data-v-cd2ec5b6><h2 class="help-title" data-v-cd2ec5b6>Need Help?</h2><div class="help-links" data-v-cd2ec5b6><a href="manual/quick-start" class="help-link" data-v-cd2ec5b6><span class="help-icon" data-v-cd2ec5b6>🚀</span><span data-v-cd2ec5b6>Quick Start Guide</span></a><a href="https://github.com/M-CreativeLab/jsar-runtime/issues" class="help-link" target="_blank" data-v-cd2ec5b6><span class="help-icon" data-v-cd2ec5b6>🐛</span><span data-v-cd2ec5b6>GitHub Issues</span></a><a href="https://github.com/M-CreativeLab/jsar-runtime/discussions" class="help-link" target="_blank" data-v-cd2ec5b6><span class="help-icon" data-v-cd2ec5b6>💬</span><span data-v-cd2ec5b6>Community Discussions</span></a></div></div></section>',1))]))}},zy=be(By,[["__scopeId","data-v-cd2ec5b6"]]),Vy={__name:"ThreeDFirstDemo",setup(i){const e=We(null);let t,n,s,r,a=[],o=new et,l=new Yf,c=null;const u=()=>{if(!e.value)return;t=new xr,n=new Wt(75,1,.1,1e3),s=new yr({antialias:!0,alpha:!0});const m=400;s.setSize(m,m),s.setClearColor(0,0),s.shadowMap.enabled=!0,s.shadowMap.type=_o,e.value.appendChild(s.domElement),n.position.set(4,3,10),n.lookAt(0,0,0);const f=new Io(4210752,.5);t.add(f);const E=new er(16777215,.8);E.position.set(8,8,8),E.castShadow=!0,E.shadow.mapSize.width=2048,E.shadow.mapSize.height=2048,t.add(E),s.domElement.addEventListener("mousemove",h),s.domElement.addEventListener("click",p),d(),g()},d=()=>{[{name:"Header",color:6514417,initialPosition:[0,4,0],finalPosition:[0,4,0],size:[10,1.8,.02],textColor:"#ffffff"},{name:"Navigation",color:1096065,initialPosition:[0,2.5,0],finalPosition:[0,2.5,.3],size:[10,1.2,.02],textColor:"#ffffff"},{name:"Hero Section",color:3900150,initialPosition:[0,.5,0],finalPosition:[0,.5,.6],size:[9.5,3.2,.02],textColor:"#ffffff"},{name:"Content Area",color:16569165,initialPosition:[-2,-1.5,0],finalPosition:[-2,-1.5,.9],size:[5.5,3.5,.02],textColor:"#ffffff"},{name:"Sidebar",color:16096779,initialPosition:[2.8,-1.5,0],finalPosition:[2.8,-1.5,1.2],size:[2.8,3.5,.02],textColor:"#ffffff"},{name:"Footer",color:9133302,initialPosition:[0,-3.8,0],finalPosition:[0,-3.8,1.5],size:[10,1.5,.02],textColor:"#ffffff"}].forEach((f,E)=>{const b=new Yn(f.size[0],f.size[1]),v=new Of({color:f.color,transparent:!0,opacity:.99,side:qt,depthTest:!1,depthWrite:!1}),T=new kt(b,v);T.renderOrder=f.finalPosition[2]*100,T.position.set(...f.initialPosition),T.castShadow=!1,T.receiveShadow=!1,t.add(T);const A=document.createElement("canvas"),C=A.getContext("2d");A.width=1536,A.height=384,C.clearRect(0,0,A.width,A.height),C.fillStyle=f.textColor,C.font="bold 140px Monospace",C.textAlign="center",C.textBaseline="middle",C.strokeStyle=f.textColor==="#ffffff"?"#000000":"#ffffff",C.lineWidth=3,C.strokeText(f.name,768,192),C.fillText(f.name,768,192);const I=new Ff(A);I.needsUpdate=!0;const y=new hs({map:I,transparent:!0,opacity:1,alphaTest:.01,depthTest:!1,depthWrite:!1,side:qt}),S=Math.min(f.size[0]*.98,6.5),P=S*.25,$=new Yn(S,P),q=new kt($,y);q.position.copy(T.position),q.position.z+=.01,q.renderOrder=T.renderOrder+.1,t.add(q),a.push({mesh:T,label:q,originalPosition:[...f.initialPosition],finalPosition:[...f.finalPosition],originalColor:f.color,name:f.name})})},h=m=>{const f=s.domElement.getBoundingClientRect();o.x=(m.clientX-f.left)/f.width*2-1,o.y=-((m.clientY-f.top)/f.height)*2+1,l.setFromCamera(o,n);const E=l.intersectObjects(a.map(b=>b.mesh));if(a.forEach(b=>{b.mesh.material.color.setHex(b.originalColor),b.mesh.scale.set(1,1,1)}),E.length>0){const b=E[0].object,v=a.find(T=>T.mesh===b);if(v){c=v;const T=new Le(v.originalColor);T.multiplyScalar(1.3),v.mesh.material.color.copy(T),v.mesh.scale.set(1.05,1.05,1),v.label.scale.set(1.05,1.05,1),s.domElement.style.cursor="pointer"}}else c=null,s.domElement.style.cursor="default"},p=m=>{if(c){const f=c;f.mesh.scale.set(.9,.9,1),f.label.scale.set(.9,.9,1),setTimeout(()=>{f.mesh.scale.set(1.05,1.05,1),f.label.scale.set(1.05,1.05,1)},100),console.log(`Clicked on: ${f.name}`)}},g=()=>{r=requestAnimationFrame(g);const m=Date.now()*.001;a.forEach((f,E)=>{const b=E*.5,A=(Math.sin(m*.8+b)+1)*.5,C=A<.5?2*A*A:1-Math.pow(-2*A+2,2)/2,I=f.originalPosition[0]+(f.finalPosition[0]-f.originalPosition[0])*C,y=f.originalPosition[1]+(f.finalPosition[1]-f.originalPosition[1])*C,S=f.originalPosition[2]+(f.finalPosition[2]-f.originalPosition[2])*C;f.mesh.position.set(I,y,S),f.label.position.set(I,y,S+.01)}),s.render(t,n)},_=()=>{r&&cancelAnimationFrame(r),s&&s.domElement&&(s.domElement.removeEventListener("mousemove",h),s.domElement.removeEventListener("click",p)),s&&(s.dispose(),e.value&&s.domElement&&e.value.removeChild(s.domElement)),t&&t.traverse(m=>{m.geometry&&m.geometry.dispose(),m.material&&(Array.isArray(m.material)?m.material.forEach(f=>f.dispose()):m.material.dispose())})};return zt(()=>{u()}),jn(()=>{_()}),(m,f)=>(L(),F("div",{ref_key:"containerRef",ref:e,class:"threejs-container"},null,512))}},Hy=be(Vy,[["__scopeId","data-v-d2ac164e"]]),la=3.2,ca=4,Gy={__name:"SpatialPhotosDemo",setup(i){const e=We(null);let t,n,s,r,a,o=0,l=0,c=0,u=0;const d=()=>{t=new xr,n=new Wt(75,e.value.clientWidth/e.value.clientHeight,.1,1e3),n.position.z=3,s=new yr({antialias:!0,alpha:!0}),s.setSize(e.value.clientWidth,e.value.clientHeight),s.setClearColor(0,0),e.value.appendChild(s.domElement);const m=new Yn(la,ca),E=new Wf().load("https://ar.rokidcdn.com/web-assets/pages/features_spatial_photos.png",T=>{T.wrapS=Cn,T.wrapT=Cn,T.minFilter=an,T.magFilter=an}),b=new on({uniforms:{map:{value:E},cornerRadius:{value:.05},blurStrength:{value:8},focusRadius:{value:.47},maxBlurRadius:{value:.9}},vertexShader:`
      varying vec2 vUv;
      void main() {
        vUv = uv;
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
      }
    `,fragmentShader:`
      uniform sampler2D map;
      uniform float cornerRadius;
      uniform float blurStrength;
      uniform float focusRadius;
      uniform float maxBlurRadius;
      varying vec2 vUv;
      
      // Rounded corner mask function
      float roundedRectMask(vec2 uv, float radius) {
        vec2 center = vec2(0.5, 0.5);
        vec2 pos = abs(uv - center);
        vec2 size = vec2(0.5, 0.5);
        
        vec2 corner = max(pos - size + radius, 0.0);
        float dist = length(corner) - radius;
        return 1.0 - smoothstep(-0.01, 0.01, dist);
      }
      
      // Apple spatial photo style depth of field blur
      vec4 appleSpacialBlur(sampler2D tex, vec2 uv, float strength) {
        vec2 center = vec2(0.5, 0.5);
        float distToCenter = distance(uv, center);
        
        // Create Apple-style depth mapping
        float depthBlur = 0.0;
        
        if (distToCenter <= focusRadius) {
          // Focus area: completely clear
          depthBlur = 0.0;
        } else if (distToCenter <= maxBlurRadius) {
          // Transition area: progressive blur, using rounded rectangle area
          float normalizedDist = (distToCenter - focusRadius) / (maxBlurRadius - focusRadius);
          
          // Use rounded rectangle to define blur area
          vec2 rectPos = abs(uv - center);
          vec2 rectSize = vec2(focusRadius, focusRadius);
          vec2 rectCorner = max(rectPos - rectSize + cornerRadius * 0.5, 0.0);
          float rectDist = length(rectCorner) - cornerRadius * 0.5;
          
          // Combine circular distance and rectangular distance
          float blendFactor = smoothstep(-0.05, 0.05, rectDist);
          depthBlur = smoothstep(0.0, 1.0, normalizedDist) * blendFactor;
        } else {
          // Background area: maximum blur
          depthBlur = 1.0;
        }
        
        // If in focus area, return original image
        if (depthBlur < 0.02) {
          return texture2D(tex, uv);
        }
        
        // Apple-style dynamic blur radius
        float dynamicBlurSize = depthBlur * strength * 0.015;
        
        vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
        float totalWeight = 0.0;
        
        // Use 7x7 sampling kernel to simulate Apple's high-quality blur
        for (int x = -3; x <= 3; x++) {
          for (int y = -3; y <= 3; y++) {
            vec2 offset = vec2(float(x), float(y)) * dynamicBlurSize;
            vec2 sampleUV = uv + offset;
            
            if (sampleUV.x >= 0.0 && sampleUV.x <= 1.0 && 
                sampleUV.y >= 0.0 && sampleUV.y <= 1.0) {
              float distance = length(vec2(float(x), float(y)));
              float weight = exp(-distance * distance * 0.12);

              // Edge enhancement, simulate real lens bokeh characteristics
              if (distance > 2.5) {
                weight *= 1.2;
              }
              color += texture2D(tex, sampleUV) * weight;
              totalWeight += weight;
            }
          }
        }

        vec4 blurredColor = color / totalWeight;
        
        // Apple-style color processing
        // Increase contrast and saturation
        blurredColor.rgb = mix(blurredColor.rgb, 
          pow(blurredColor.rgb, vec3(0.92)), depthBlur * 0.25);
        
        // Add slight color temperature shift and vignetting
        blurredColor.rgb = mix(blurredColor.rgb, 
          blurredColor.rgb * vec3(1.02, 1.0, 0.98), depthBlur * 0.15);
        
        // Add slight vignetting effect
        float vignette = 1.0 - smoothstep(0.4, 0.8, distToCenter);
        blurredColor.rgb *= mix(0.92, 1.0, vignette);
        return blurredColor;
      }

      void main() {
        vec2 uv = vUv;
        float mask = roundedRectMask(uv, cornerRadius);
        
        if (mask < 0.01) {
          discard;
        }
        
        vec4 finalColor = appleSpacialBlur(map, uv, blurStrength);
        gl_FragColor = vec4(finalColor.rgb, finalColor.a * mask);
      }
    `,transparent:!0});r=new kt(m,b),t.add(r),h();const v=new Io(4210752,.8);t.add(v)},h=()=>{const m=new Yn(la,ca),f=new on({uniforms:{time:{value:0},uTexture:{value:null},resolution:{value:new et(la,ca)},blurStrength:{value:24},cornerRadius:{value:.08},focusPoint:{value:new et(.5,.5)},focusRadius:{value:.15},maxBlurRadius:{value:.45}},vertexShader:`
      varying vec2 vUv;
      void main() {
        vUv = uv;
        gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
      }
    `,fragmentShader:`
      uniform float time;
      uniform sampler2D uTexture;
      uniform vec2 resolution;
      uniform float blurStrength;
      uniform float cornerRadius;
      uniform vec2 focusPoint;
      uniform float focusRadius;
      uniform float maxBlurRadius;
      varying vec2 vUv;
      
      float roundedRectMask(vec2 uv, float radius) {
        vec2 center = vec2(0.5, 0.5);
        vec2 pos = abs(uv - center);
        vec2 size = vec2(0.5, 0.5);
        
        vec2 corner = max(pos - size + radius, 0.0);
        float dist = length(corner) - radius;
        
        return 1.0 - smoothstep(-0.003, 0.003, dist);
      }
      
      vec4 visionProBlur(sampler2D tex, vec2 uv, float strength) {
        // Calculate distance to focus point
        float distToFocus = distance(uv, focusPoint);
        
        // Create Vision Pro style depth mapping
        float depthBlur = 0.0;
        
        if (distToFocus <= focusRadius) {
          // Focus area: completely clear
          depthBlur = 0.0;
        } else if (distToFocus <= maxBlurRadius) {
          // Transition area: progressive blur
          float normalizedDist = (distToFocus - focusRadius) / (maxBlurRadius - focusRadius);
          depthBlur = smoothstep(0.0, 1.0, normalizedDist);
        } else {
          // Background area: maximum blur
          depthBlur = 1.0;
        }
        
        // If in focus area, return original image
        if (depthBlur < 0.02) {
          return texture2D(tex, uv);
        }
        
        // Dynamic blur radius, simulate real lens effect
        float dynamicBlurSize = depthBlur * strength * 0.025;
        
        vec4 color = vec4(0.0);
        float totalWeight = 0.0;
        
        // Use larger sampling kernel (9x9) to simulate Vision Pro's high-quality blur
        int sampleRadius = int(mix(1.0, 4.0, depthBlur));
        
        for (int x = -4; x <= 4; x++) {
          for (int y = -4; y <= 4; y++) {
            if (abs(x) > sampleRadius || abs(y) > sampleRadius) continue;
            
            vec2 offset = vec2(float(x), float(y)) * dynamicBlurSize;
            vec2 sampleUV = uv + offset;
            
            if (sampleUV.x >= 0.0 && sampleUV.x <= 1.0 && 
                sampleUV.y >= 0.0 && sampleUV.y <= 1.0) {
              
              // Vision Pro style bokeh weight
              float distance = length(vec2(float(x), float(y)));
              float weight = exp(-distance * distance * 0.15);
              
              // Add hexagonal bokeh effect
              float angle = atan(float(y), float(x));
              float hexWeight = 1.0 + 0.2 * cos(angle * 6.0);
              weight *= hexWeight;
              
              // Edge enhancement, simulate real lens bokeh characteristics
              if (distance > 2.0) {
                weight *= 1.3;
              }
              
              color += texture2D(tex, sampleUV) * weight;
              totalWeight += weight;
            } // Add this missing closing brace
          }
        }
        
        vec4 blurredColor = color / totalWeight;
        blurredColor.rgb = mix(blurredColor.rgb, 
          pow(blurredColor.rgb, vec3(0.9)), depthBlur * 0.3);
        
        blurredColor.rgb = mix(blurredColor.rgb, 
          blurredColor.rgb * vec3(1.02, 1.0, 0.98), depthBlur * 0.2);
        
        return blurredColor;
      }
      
      void main() {
        vec2 uv = vUv;

        float mask = roundedRectMask(uv, cornerRadius);
        if (mask < 0.01) {
          discard;
        }
        
        // Apply Vision Pro style blur
        vec4 finalColor = visionProBlur(uTexture, uv, blurStrength);
        
        // Add slight vignetting effect to enhance spatial sense
        vec2 center = vec2(0.5, 0.5);
        float vignette = 1.0 - smoothstep(0.3, 0.8, distance(uv, center));
        finalColor.rgb *= mix(0.85, 1.0, vignette);
        
        // Apply mask
        gl_FragColor = vec4(finalColor.rgb, finalColor.a * mask);
      }
    `,transparent:!0,depthWrite:!1});r&&r.material.map&&(f.uniforms.uTexture.value=r.material.map),a=new kt(m,f),a.position.z=.01,t.add(a)},p=m=>{const f=e.value.getBoundingClientRect();o=(m.clientX-f.left)/f.width*2-1,l=-((m.clientY-f.top)/f.height)*2+1,c=o*2,u=l*1},g=()=>{requestAnimationFrame(g),n.position.x+=(c-n.position.x)*.05,n.position.y+=(u-n.position.y)*.05,n.lookAt(0,0,0),r&&(r.rotation.y=Math.sin(Date.now()*.001)*.1,r.rotation.x=Math.cos(Date.now()*.0015)*.05),a&&(a.material.uniforms.time.value=Date.now()*.001,a.rotation.y=Math.sin(Date.now()*.001)*.1,a.rotation.x=Math.cos(Date.now()*.0015)*.05),s.render(t,n)},_=()=>{e.value&&(n.aspect=e.value.clientWidth/e.value.clientHeight,n.updateProjectionMatrix(),s.setSize(e.value.clientWidth,e.value.clientHeight))};return zt(()=>{d(),g(),e.value.addEventListener("mousemove",p),window.addEventListener("resize",_)}),jn(()=>{e.value&&e.value.removeEventListener("mousemove",p),window.removeEventListener("resize",_),s&&s.dispose()}),(m,f)=>(L(),F("div",{ref_key:"containerRef",ref:e,class:"spatial-window-demo"},null,512))}},Wy=be(Gy,[["__scopeId","data-v-65847283"]]),$y={class:"audio-visualizer-demo"},Xy={class:"controls"},qy={class:"distance-control"},Yy={class:"audio-control"},jy={__name:"AudioVisualizerDemo",setup(i){const e=We(null),t=We(10),n=We(!1),s=We(0);let r,a,o,l,c,u,d,h,p,g,_,m,f=null,E=[];const b=[new Le(31487),new Le(16726832),new Le(3458905),new Le(16749824),new Le(11490014),new Le(16723346),new Le(5949690),new Le(16763904)];let v=0,T=0,A=1,C=0;const I=()=>{for(let B=0;B<8;B++){const Q=new Mr(.8+B*.15,1+B*.15,64),G=new on({uniforms:{time:{value:0},audioData:{value:.1},ringIndex:{value:B},opacity:{value:.8-B*.1},waveColor:{value:new Le(31487)}},vertexShader:`
        uniform float time;
        uniform float audioData;
        uniform float ringIndex;
        varying vec2 vUv;
        varying float vWave;
        varying vec2 vPosition;
        
        void main() {
          vUv = uv;
          vPosition = position.xy;
          
          vec3 pos = position;
          
          // Create wave effect - based on audio data and time
          float angle = atan(pos.y, pos.x);
          float radius = length(pos.xy);
          
          // Multi-layer wave overlay
          float wave1 = sin(angle * 8.0 + time * 2.0 - ringIndex * 0.5) * audioData;
          float wave2 = cos(angle * 12.0 + time * 1.5 - ringIndex * 0.3) * audioData * 0.5;
          float wave3 = sin(angle * 16.0 + time * 3.0 - ringIndex * 0.8) * audioData * 0.3;
          
          float totalWave = (wave1 + wave2 + wave3) * 0.2; // Reduce deformation amplitude
          vWave = totalWave;
          
          // Apply wave deformation
          pos.xy *= (1.0 + totalWave);
          
          // Outward expansion animation
          float expansionPhase = mod(time * 0.8 + ringIndex * 0.4, 2.0);
          float expansion = smoothstep(0.0, 0.3, expansionPhase) * (1.0 - smoothstep(1.7, 2.0, expansionPhase));
          pos.xy *= (1.0 + expansion * 0.3); // Reduce expansion amplitude
          
          gl_Position = projectionMatrix * modelViewMatrix * vec4(pos, 1.0);
        }
      `,fragmentShader:`
        uniform float time;
        uniform float audioData;
        uniform float opacity;
        uniform vec3 waveColor;
        varying vec2 vUv;
        varying float vWave;
        varying vec2 vPosition;
        
        void main() {
          // Color based on wave intensity
          vec3 color = waveColor;
          
          // Adjust color intensity based on audio data
          color *= (0.8 + audioData * 1.5); // Ensure base brightness
          
          // Simplified radial gradient effect
          float distanceFromCenter = length(vPosition);
          float radialGradient = 1.0 - smoothstep(0.5, 1.5, distanceFromCenter);
          
          // Simplified transparency calculation, ensure visibility
          float baseAlpha = opacity * 0.6; // Base transparency
          float waveAlpha = abs(vWave) * 0.4; // Wave enhancement
          float audioAlpha = audioData * 0.3; // Audio response
          
          float alpha = baseAlpha + waveAlpha + audioAlpha;
          alpha = clamp(alpha, 0.1, 1.0); // Ensure minimum visibility
          
          gl_FragColor = vec4(color, alpha * radialGradient);
        }
      `,transparent:!0,side:qt,blending:sr,depthWrite:!1}),ie=new kt(Q,G);ie.position.set(0,0,0),ie.rotation.x=0,r.add(ie),E.push({mesh:ie,material:G,initialScale:1+B*.1})}},y=()=>{r=new xr;const X=e.value.clientWidth,B=e.value.clientHeight,Q=X/B,G=Q<1?85:75;a=new Wt(G,Q,.1,1e3),a.position.set(0,0,t.value),a.lookAt(0,0,0),o=new yr({canvas:e.value,antialias:!0,alpha:!0}),o.setSize(X,B),o.setPixelRatio(Math.min(window.devicePixelRatio,2)),o.setClearColor(0,0),o.shadowMap.enabled=!0,o.shadowMap.type=_o,l=new Po(1,2),c=new on({uniforms:{time:{value:0},audioData:{value:0},primaryColor:{value:new Le(31487)},secondaryColor:{value:new Le(16726832)},accentColor:{value:new Le(16777215)},metallic:{value:.8},roughness:{value:.2}},vertexShader:`
      uniform float time;
      uniform float audioData;
      varying vec3 vPosition;
      varying vec3 vNormal;
      varying vec3 vWorldPosition;
      
      void main() {
        vPosition = position;
        vNormal = normalize(normalMatrix * normal);
        
        // Create more natural wave deformation based on audio data
        vec3 newPosition = position;
        float wave1 = sin(position.y * 8.0 + time * 1.5) * audioData * 0.15;
        float wave2 = cos(position.x * 6.0 + time * 2.0) * audioData * 0.1;
        float wave3 = sin(position.z * 10.0 + time * 1.8) * audioData * 0.08;
        
        newPosition += normal * (wave1 + wave2 + wave3);
        
        vec4 worldPosition = modelMatrix * vec4(newPosition, 1.0);
        vWorldPosition = worldPosition.xyz;
        
        gl_Position = projectionMatrix * modelViewMatrix * vec4(newPosition, 1.0);
      }
    `,fragmentShader:`
      uniform float time;
      uniform float audioData;
      uniform vec3 primaryColor;
      uniform vec3 secondaryColor;
      uniform vec3 accentColor;
      uniform float metallic;
      uniform float roughness;
      
      varying vec3 vPosition;
      varying vec3 vNormal;
      varying vec3 vWorldPosition;
      
      // Apple-style Fresnel reflection
      float fresnel(vec3 viewDirection, vec3 normal, float power) {
        return pow(1.0 - max(0.0, dot(viewDirection, normal)), power);
      }
      
      void main() {
        vec3 viewDirection = normalize(cameraPosition - vWorldPosition);
        vec3 normal = normalize(vNormal);
        
        // Base color mixing - Apple-style gradient
        float colorMix = sin(vPosition.y * 3.0 + time * 0.5) * 0.5 + 0.5;
        vec3 baseColor = mix(primaryColor, secondaryColor, colorMix + audioData * 0.3);
        
        // Fresnel effect - edge glow
        float fresnelFactor = fresnel(viewDirection, normal, 2.0);
        vec3 fresnelColor = accentColor * fresnelFactor;
        
        // Metallic reflection
        float metallicReflection = metallic * (1.0 - roughness);
        vec3 reflectionColor = mix(baseColor, accentColor, metallicReflection * fresnelFactor);
        
        // Audio-responsive glow effect
        vec3 glowColor = primaryColor * audioData * 2.0;
        
        // Final color composition
        vec3 finalColor = baseColor + fresnelColor * 0.8 + glowColor;
        finalColor = mix(finalColor, reflectionColor, 0.3);
        
        // Apple-style transparency - glass texture
        float alpha = 0.85 + audioData * 0.15 + fresnelFactor * 0.2;
        
        gl_FragColor = vec4(finalColor, alpha);
      }
    `,transparent:!0,side:qt}),u=new kt(l,c),u.position.set(0,0,0),u.castShadow=!0,u.receiveShadow=!0,r.add(u),I();const ie=new er(16777215,1.2);ie.position.set(5,5,5),ie.castShadow=!0,ie.shadow.mapSize.width=2048,ie.shadow.mapSize.height=2048,r.add(ie);const de=new Io(4210752,.4);r.add(de);const Ee=new er(31487,.8);Ee.position.set(-3,2,-3),r.add(Ee);const Oe=new er(16777215,.3);Oe.position.set(0,-5,0),r.add(Oe),$()},S=async()=>{try{d=new(window.AudioContext||window.webkitAudioContext),h=d.createAnalyser(),h.fftSize=512,m=new Uint8Array(h.frequencyBinCount),console.info("Audio context initialized successfully, state:",d.state);const X="https://ar.rokidcdn.com/web-assets/pages/sounds/music.wav";try{console.info("Starting to load remote audio:",X);const B=await fetch(X);if(!B.ok)throw new Error(`HTTP error! status: ${B.status}`);const Q=await B.arrayBuffer();p=await d.decodeAudioData(Q),console.info("Remote audio loaded successfully:",p.duration+" seconds, sample rate:",p.sampleRate)}catch(B){console.warn("Remote audio loading failed, using backup audio:",B);const Q=d.sampleRate,ie=Q*15;p=d.createBuffer(2,ie,Q);for(let de=0;de<p.numberOfChannels;de++){const Ee=p.getChannelData(de);for(let Oe=0;Oe<ie;Oe++){const at=Oe/Q;Ee[Oe]=Math.sin(2*Math.PI*220*at)*.2+Math.sin(2*Math.PI*440*at)*.15+Math.sin(2*Math.PI*660*at)*.1+Math.sin(2*Math.PI*880*at)*.08+Math.sin(2*Math.PI*110*at)*.12*(de===0?1:.8)}}console.info("Backup audio generated successfully:",p.duration+" seconds")}}catch(X){console.error("Audio initialization failed:",X)}},P=async()=>{if(!d||!p){console.error("Audio context or buffer not initialized");return}if(n.value)g&&(g.stop(),g=null,f=null),n.value=!1,console.info("Audio stopped");else try{d.state==="suspended"&&await d.resume(),g=d.createBufferSource(),g.buffer=p,g.loop=!0,f=d.createGain();const X=s.value;f.gain.value=X,console.info("Set volume:",X),g.connect(f),f.connect(h),h.connect(d.destination),g.onended=()=>{console.info("Audio playback ended")},g.start(),console.info("Audio started playing, buffer duration:",p.duration,"seconds"),n.value=!0}catch(X){console.error("Audio playback failed:",X),n.value=!1}},$=()=>{if(a){a.position.set(0,0,parseFloat(t.value)),a.lookAt(0,0,0);const X=20,B=2,Q=(t.value-B)/(X-B),G=.05,ie=2;s.value=G+(ie-G)*Math.pow(1-Q,1.5),console.info("Distance:",t.value,"Volume:",s.value.toFixed(2)),n.value&&f&&(f.gain.setValueAtTime(s.value,d.currentTime),console.info("Real-time volume updated to:",s.value.toFixed(2)))}},q=()=>{if(_=requestAnimationFrame(q),h&&m){h.getByteFrequencyData(m);const X=Math.floor(m.length*.1),B=Math.floor(m.length*.5),Q=m.length;let G=0,ie=0,de=0;for(let He=0;He<X;He++)G+=m[He];for(let He=X;He<B;He++)ie+=m[He];for(let He=B;He<Q;He++)de+=m[He];const Ee=G/X/255,Oe=ie/(B-X)/255,at=de/(Q-B)/255;if(c&&c.uniforms&&(c.uniforms.time.value=performance.now()*.001,c.uniforms.audioData.value=Oe),E.length>0&&(v+=.016,v>2&&(v=0,T=A,A=(A+1)%b.length,C=0),C=Math.min(C+.02,1),E.forEach((He,j)=>{const he=Math.floor(j/E.length*m.length),le=m[he]/255;He.material.uniforms.time.value=performance.now()*.001,He.material.uniforms.audioData.value=Math.max(.1,le);const Be=He.initialScale*(1+le*.3);if(He.mesh.scale.setScalar(Be),n.value&&le>.1){const Ue=b[T],$e=b[A],Mt=.5+le*1.5,st=Cr.lerp(Ue.r,$e.r,C)*Mt,D=Cr.lerp(Ue.g,$e.g,C)*Mt,ft=Cr.lerp(Ue.b,$e.b,C)*Mt,Ne=(j*.1+performance.now()*5e-4)%1,rt=new Le().setHSL((st+D+ft)/3+Ne,.8,.6+le*.4);He.material.uniforms.waveColor.value=rt}else He.material.uniforms.waveColor.value=new Le(31487).multiplyScalar(.5)})),u){u.rotation.y+=Ee*.02,u.rotation.x+=at*.01;const He=1+Oe*.3;u.scale.setScalar(He);const j=Math.sin(performance.now()*.001)*Oe*.3;u.position.set(0,j,0)}}o&&r&&a&&o.render(r,a)},Y=()=>{if(!a||!o||!e.value)return;const X=e.value.clientWidth,B=e.value.clientHeight,Q=X/B,G=Q<1?85:75;a.fov=G,a.aspect=Q,a.updateProjectionMatrix(),a.position.set(0,0,t.value),a.lookAt(0,0,0),o.setSize(X,B),o.setPixelRatio(Math.min(window.devicePixelRatio,2)),u&&u.position.set(0,0,0),E.forEach(ie=>{ie.mesh&&ie.mesh.position.set(0,0,0)})};return zt(async()=>{await S(),y(),q(),window.addEventListener("resize",Y)}),jn(()=>{_&&cancelAnimationFrame(_),g&&g.stop(),d&&d.close(),E.forEach(X=>{X.mesh&&(r.remove(X.mesh),X.mesh.geometry.dispose(),X.mesh.material.dispose())}),E=[],o&&o.dispose(),window.removeEventListener("resize",Y)}),(X,B)=>(L(),F("div",$y,[w("canvas",{ref_key:"canvasRef",ref:e,class:"visualizer-canvas",onTouchstart:B[0]||(B[0]=(...Q)=>X.handleTouchStart&&X.handleTouchStart(...Q)),onTouchmove:B[1]||(B[1]=(...Q)=>X.handleTouchMove&&X.handleTouchMove(...Q)),onTouchend:B[2]||(B[2]=(...Q)=>X.handleTouchEnd&&X.handleTouchEnd(...Q))},null,544),w("div",Xy,[w("div",qy,[B[4]||(B[4]=w("label",null,"Distance",-1)),Ui(w("input",{type:"range",min:"2",max:"20",step:"0.1","onUpdate:modelValue":B[3]||(B[3]=Q=>t.value=Q),onInput:$,class:"distance-slider"},null,544),[[fo,t.value]])]),w("div",Yy,[w("button",{onClick:P,class:"audio-button"},xe(n.value?"Pause":"Play"),1)])])]))}},Zy=be(jy,[["__scopeId","data-v-74e3e1b7"]]),Ky={class:"spatial-web-features"},Jy={class:"features-container"},Qy={class:"feature-content"},eb={class:"feature-left"},tb={class:"feature-header"},nb={class:"icon-emoji-large"},ib={class:"feature-text"},sb={class:"feature-title"},rb={class:"feature-subtitle"},ab={class:"feature-description"},ob={key:0,class:"feature-demo"},lb={class:"demo-window"},cb={class:"demo-header"},ub={class:"demo-title"},db={class:"demo-content"},hb={class:"feature-right"},fb={class:"feature-visual"},pb={class:"visual-container"},mb={__name:"SpatialWebFeatures",setup(i){const e=[{id:"3d-design",icon:"🎨",title:"3D-First Design",subtitle:"Content designed for 3D space",description:"Unlike traditional flat web pages, spatial web content is inherently three-dimensional, allowing for depth, perspective, and immersive layouts that feel natural in 3D environments.",codeExample:`// Spatialized elements
<div style="transform: translateZ(100)">
  <h1>Welcome to Spatial Web</h1>
</div>`,color:"from-blue-500 to-purple-600",bgColor:"bg-blue-50 dark:bg-blue-950/20"},{id:"spatial-image",icon:"🖼️",title:"Spatial Photos",subtitle:"Presenting spatial photos easily",description:"Rendering spatial photos has never been easier - just use the <img> element for creator",codeExample:'<img src="example.jpg" spatial="stereo" />',color:"from-green-500 to-teal-600",bgColor:"bg-green-50 dark:bg-green-950/20"},{id:"spatial-audio",icon:"🔊",title:"Audio Spatialization",subtitle:"Natively support audio spatialization",description:"<audio> elements with spatialization are automatically enabled, the only thing for the creator is to add the audio element and set the source.",codeExample:`// Audio spatialization
<audio src="example.mp3"></audio>
`,color:"from-orange-500 to-red-600",bgColor:"bg-orange-50 dark:bg-orange-950/20"}],t=We(new Set),n=We(0),s=()=>{const a=new IntersectionObserver(o=>{o.forEach(l=>{const c=parseInt(l.target.dataset.index);l.isIntersecting&&(t.value.add(c),n.value=c)})},{threshold:.3,rootMargin:"-10% 0px -10% 0px"});return document.querySelectorAll(".feature-section").forEach((o,l)=>{o.dataset.index=l,a.observe(o)}),a};let r=null;return zt(()=>{setTimeout(()=>{r=s()},100)}),jn(()=>{r&&r.disconnect()}),(a,o)=>(L(),F("div",Ky,[w("div",Jy,[(L(),F(tt,null,vt(e,(l,c)=>w("div",{key:l.id,class:ct(["feature-section",{reverse:c%2===1,visible:t.value.has(c)}])},[w("div",Qy,[w("div",eb,[w("div",tb,[w("div",{class:ct(["feature-icon-large",`bg-gradient-to-br ${l.color}`])},[w("span",nb,xe(l.icon),1)],2),w("div",ib,[w("h3",sb,xe(l.title),1),w("p",rb,xe(l.subtitle),1)])]),w("p",ab,xe(l.description),1),l.codeExample?(L(),F("div",ob,[w("div",lb,[w("div",cb,[o[0]||(o[0]=w("div",{class:"demo-controls"},[w("span",{class:"demo-dot red"}),w("span",{class:"demo-dot yellow"}),w("span",{class:"demo-dot green"})],-1)),w("span",ub,xe(l.title),1)]),w("div",db,[w("pre",null,[w("code",null,xe(l.codeExample),1)])])])])):_e("",!0)]),w("div",hb,[w("div",fb,[w("div",pb,[l.id==="3d-design"?(L(),De(Hy,{key:0})):l.id==="spatial-image"?(L(),De(Wy,{key:1})):l.id==="spatial-audio"?(L(),De(Zy,{key:2})):_e("",!0)])])])])],2)),64))])]))}},gb=be(mb,[["__scopeId","data-v-ff0c8b20"]]),_b={},vb={class:"call-to-action"};function xb(i,e){return L(),F("div",vb,e[0]||(e[0]=[mc('<div class="cta-content" data-v-f1c90a2d><h3 class="cta-title" data-v-f1c90a2d>Ready to try Spatial Web?</h3><p class="cta-text" data-v-f1c90a2d> JSAR browser engine brings all these Spatial Web capabilities to your fingertips. </p><div class="cta-actions" data-v-f1c90a2d><a href="manual/quick-start.html" class="cta-button primary" data-v-f1c90a2d> Get Started <svg class="arrow-icon" viewBox="0 0 20 20" fill="currentColor" data-v-f1c90a2d><path fill-rule="evenodd" d="M7.293 14.707a1 1 0 010-1.414L10.586 10 7.293 6.707a1 1 0 011.414-1.414l4 4a1 1 0 010 1.414l-4 4a1 1 0 01-1.414 0z" clip-rule="evenodd" data-v-f1c90a2d></path></svg></a><a href="api/browser-compat-data.html" class="cta-button secondary" data-v-f1c90a2d>View API Docs</a></div></div>',1)]))}const Mb=be(_b,[["render",xb],["__scopeId","data-v-f1c90a2d"]]),Sb={name:"GalleryPage",data(){return{fixturesData:null,searchQuery:"",selectedCategory:"all",loading:!0,error:null,filteredFixtures:[]}},computed:{categories(){var i;return((i=this.fixturesData)==null?void 0:i.categories)||[]},allFixtures(){var i;return((i=this.fixturesData)==null?void 0:i.categories.flatMap(e=>e.fixtures))||[]}},async mounted(){await this.loadFixturesData(),this.filterFixtures()},methods:{async loadFixturesData(){try{this.loading=!0;const i=await fetch("fixtures-summary.json");if(!i.ok)throw new Error(`HTTP ${i.status}: ${i.statusText}`);this.fixturesData=await i.json()}catch(i){console.error("Failed to load fixtures data:",i),this.error=i.message}finally{this.loading=!1}},filterFixtures(){const i=this.searchQuery.toLowerCase().trim();let e=this.allFixtures;this.selectedCategory!=="all"&&(e=e.filter(t=>t.category===this.selectedCategory)),i&&(e=e.filter(t=>t.filename.toLowerCase().includes(i)||t.title.toLowerCase().includes(i)||t.description&&t.description.toLowerCase().includes(i))),this.filteredFixtures=e},clearSearch(){this.searchQuery="",this.selectedCategory="all",this.filterFixtures()},filterByCategory(i){this.selectedCategory=i,this.searchQuery="",this.filterFixtures(),this.$el.scrollIntoView({behavior:"smooth"})},openExample(i){window.webview&&window.webview.raiseEvent?window.webview.raiseEvent("requestSpatialPresentation",i):window.open(i,"_blank")}}},yb={class:"gallery-container"},bb={class:"gallery-header"},Eb={class:"description"},Tb={class:"gallery-controls"},wb={class:"search-box"},Ab={class:"filter-container"},Rb={class:"filter-group"},Cb=["value"],Pb={class:"stats-container"},Lb={class:"stat-item"},Db={class:"stat-value"},Ib={class:"stat-label"},Ub={class:"stat-item"},kb={class:"stat-value"},Nb={key:0,class:"loading-state"},Fb={key:1,class:"error-state"},Ob={key:2,class:"no-results"},Bb={key:3,class:"examples-grid"},zb={class:"card-header"},Vb=["title"],Hb={class:"example-category"},Gb={class:"card-body"},Wb=["title"],$b={key:0,class:"example-description"},Xb={class:"card-actions"},qb=["onClick"],Yb=["href"],jb={class:"categories-section"},Zb={class:"categories-grid"},Kb=["onClick"];function Jb(i,e,t,n,s,r){var a,o;return L(),F("div",yb,[w("header",bb,[e[6]||(e[6]=w("h1",null,"JSAR Examples Gallery",-1)),w("p",Eb,"Browse and discover "+xe(((a=s.fixturesData)==null?void 0:a.total)||0)+" HTML examples and test pages.",1)]),w("div",Tb,[w("div",wb,[Ui(w("input",{type:"text","onUpdate:modelValue":e[0]||(e[0]=l=>s.searchQuery=l),placeholder:"Search examples by name, title, or description...",onInput:e[1]||(e[1]=(...l)=>r.filterFixtures&&r.filterFixtures(...l)),class:"search-input"},null,544),[[fo,s.searchQuery]]),s.searchQuery?(L(),F("button",{key:0,onClick:e[2]||(e[2]=(...l)=>r.clearSearch&&r.clearSearch(...l)),class:"clear-button"}," ✕ ")):_e("",!0)]),w("div",Ab,[w("div",Rb,[e[8]||(e[8]=w("label",{for:"category-filter"},"Category:",-1)),Ui(w("select",{id:"category-filter","onUpdate:modelValue":e[3]||(e[3]=l=>s.selectedCategory=l),onChange:e[4]||(e[4]=(...l)=>r.filterFixtures&&r.filterFixtures(...l)),class:"filter-select"},[e[7]||(e[7]=w("option",{value:"all"},"All Categories",-1)),(L(!0),F(tt,null,vt(r.categories,l=>(L(),F("option",{key:l.name,value:l.name},xe(l.name)+" ("+xe(l.count)+") ",9,Cb))),128))],544),[[pa,s.selectedCategory]])])])]),w("div",Pb,[w("div",Lb,[w("div",Db,xe(s.filteredFixtures.length),1),w("div",Ib,xe(s.filteredFixtures.length===((o=s.fixturesData)==null?void 0:o.total)?"Total":"Filtered")+" Examples",1)]),w("div",Ub,[w("div",kb,xe(r.categories.length),1),e[9]||(e[9]=w("div",{class:"stat-label"},"Categories",-1))])]),s.loading?(L(),F("div",Nb,e[10]||(e[10]=[w("div",{class:"loading-spinner"},null,-1),w("p",null,"Loading examples...",-1)]))):s.error?(L(),F("div",Fb,[w("p",null,"Failed to load examples: "+xe(s.error),1)])):s.filteredFixtures.length===0&&s.searchQuery?(L(),F("div",Ob,[w("p",null,'No examples found matching "'+xe(s.searchQuery)+'"',1),w("button",{onClick:e[5]||(e[5]=(...l)=>r.clearSearch&&r.clearSearch(...l)),class:"clear-search-btn"},"Clear search")])):(L(),F("div",Bb,[(L(!0),F(tt,null,vt(s.filteredFixtures,l=>(L(),F("div",{key:l.filename,class:"example-card"},[w("div",zb,[w("h3",{class:"example-title",title:l.title},xe(l.title),9,Vb),w("span",Hb,xe(l.category),1)]),w("div",Gb,[w("div",{class:"example-filename",title:l.filename},[w("code",null,xe(l.filename),1)],8,Wb),l.description?(L(),F("p",$b,xe(l.description),1)):_e("",!0)]),w("div",Xb,[w("a",{onClick:c=>r.openExample(l.rawUrl),target:"_blank",class:"action-btn primary"}," Open ",8,qb),w("a",{href:l.githubUrl,target:"_blank",class:"action-btn secondary"}," View Source ",8,Yb)])]))),128))])),w("div",jb,[e[11]||(e[11]=w("h2",null,"Example Categories",-1)),w("div",Zb,[(L(!0),F(tt,null,vt(r.categories,l=>(L(),F("div",{key:l.name,class:"category-card",onClick:c=>r.filterByCategory(l.name)},[w("h3",null,xe(l.name),1),w("p",null,xe(l.count)+" example"+xe(l.count===1?"":"s"),1)],8,Kb))),128))])])])}const Qb=be(Sb,[["render",Jb],["__scopeId","data-v-a1221a50"]]),nE={extends:GM,enhanceApp({app:i}){i.component("LatestRelease",DS),i.component("BrowserCompatData",vy),i.component("Hero3DEffect",Zc),i.component("DownloadPage",zy),i.component("SpatialWebFeatures",gb),i.component("CallToAction",Mb),i.component("GalleryPage",Qb)}};export{nE as R,Tx as c,Qe as u};
